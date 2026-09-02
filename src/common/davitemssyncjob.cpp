/*
 *  SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "davitemssyncjob.h"

#include "daverror.h"
#include "davjobbase_p.h"
#include "davmanager_p.h"
#include "davmultigetprotocol_p.h"
#include "davurl.h"
#include "libkdav_debug.h"

#include <QNetworkReply>
#include <QXmlStreamWriter>
#include <klocalizedstring.h>

using namespace KDAV;
using namespace Qt::Literals;

namespace
{

struct PropStat {
    QString status;
    QString etag;
    bool isCollection = false;
};

void parseProp(QXmlStreamReader &reader, PropStat &propStat)
{
    while (reader.readNextStartElement()) {
        if (reader.name() == "getetag"_L1) {
            propStat.etag = reader.readElementText();
        } else if (reader.name() == "resourcetype"_L1) {
            bool isCollection = false;
            while (reader.readNextStartElement()) {
                if (reader.name() == "collection"_L1) {
                    isCollection = true;
                }
                reader.skipCurrentElement();
            }
            propStat.isCollection = isCollection;
        } else {
            reader.skipCurrentElement();
        }
    }
}

PropStat parsePropStat(QXmlStreamReader &reader)
{
    auto propStat = PropStat();
    while (reader.readNextStartElement()) {
        if (reader.name() == "status"_L1) {
            propStat.status = reader.readElementText();
        } else if (reader.name() == "prop"_L1) {
            parseProp(reader, propStat);
        } else {
            reader.skipCurrentElement();
        }
    }
    return propStat;
}
}

namespace KDAV
{
class DavItemsSyncJobPrivate : public DavJobBasePrivate
{
public:
    QString buildQuery() const;
    void davJobFinished(QNetworkReply *reply);

    DavUrl mUrl;
    QString mSyncToken;

    DavItem::List mChangedItems;
    QStringList mDeletedItems;
    QString mNewSyncToken;
};
}

DavItemsSyncJob::DavItemsSyncJob(const DavUrl &url, const QString &syncToken, QObject *parent)
    : DavJobBase(new DavItemsSyncJobPrivate, parent)
{
    Q_D(DavItemsSyncJob);
    d->mUrl = url;
    d->mSyncToken = syncToken;
}

DavItemsSyncJob::~DavItemsSyncJob() = default;

void DavItemsSyncJob::start()
{
    Q_D(DavItemsSyncJob);

    const auto props = d->buildQuery();
    auto *reply = DavManager::self()->createReportJob(this, d->mUrl.url(), props);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavItem::List DavItemsSyncJob::changedItems() const
{
    Q_D(const DavItemsSyncJob);
    return d->mChangedItems;
}

QStringList DavItemsSyncJob::deletedItems() const
{
    Q_D(const DavItemsSyncJob);
    return d->mDeletedItems;
}

QString DavItemsSyncJob::newSyncToken() const
{
    Q_D(const DavItemsSyncJob);
    return d->mNewSyncToken;
}

QString DavItemsSyncJobPrivate::buildQuery() const
{
    auto output = QString();
    auto writer = QXmlStreamWriter(&output);
    writer.setAutoFormatting(true);

    writer.writeNamespace(Xml::davNS, "D"_L1);
    writer.writeStartElement(Xml::davNS, "sync-collection"_L1);
    {
        writer.writeStartElement(Xml::davNS, "sync-token"_L1);
        if (!mSyncToken.isEmpty()) {
            writer.writeCharacters(mSyncToken);
        }
        writer.writeEndElement();

        writer.writeTextElement(Xml::davNS, "sync-level"_L1, "1"_L1);
        writer.writeStartElement(Xml::davNS, "prop"_L1);
        {
            writer.writeEmptyElement(Xml::davNS, "getetag"_L1);
            writer.writeEmptyElement(Xml::davNS, "resourcetype"_L1);
        }
        writer.writeEndElement();
    }
    writer.writeEndElement();
    return output;
}

void DavItemsSyncJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError || (responseCode >= 400 && responseCode < 600)) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
        emitResult();
        return;
    }

    // Payload example can be found in related DavItemsSyncJobTest
    auto reader = QXmlStreamReader(reply);
    while (reader.readNextStartElement()) {
        if (reader.name() == "multistatus"_L1) {
            break;
        }
        reader.skipCurrentElement();
    }

    while (reader.readNextStartElement()) {
        if (reader.name() == "sync-token"_L1) {
            mNewSyncToken = reader.readElementText();
            continue;
        }

        if (reader.name() == "response"_L1) {
            auto href = QString();
            auto status = QString();
            auto propStat = std::optional<PropStat>();
            while (reader.readNextStartElement()) {
                if (reader.name() == "href"_L1) {
                    href = reader.readElementText();
                } else if (reader.name() == "status"_L1) {
                    status = reader.readElementText();
                } else if (reader.name() == "propstat"_L1) {
                    const auto propStatCandidate = parsePropStat(reader);
                    if (propStatCandidate.status.contains(QLatin1String("200"))) {
                        propStat = propStatCandidate;
                    }
                } else {
                    reader.skipCurrentElement();
                }
            }

            // Get element's url aka. it's remoteId
            if (href.isEmpty()) {
                qCWarning(KDAV_LOG) << "DAV:response element without href element - not RFC compliant";
                setError(ERR_MALFORMED_RESPONSE);
                setErrorText(i18n("Invalid server response payload, response item missing HREF element"));
                emitResult();
                return;
            }

            QUrl url = reply->url();
            url.setUserInfo(QString());
            if (href.startsWith(QLatin1Char('/'))) {
                url.setPath(href, QUrl::TolerantMode);
            } else {
                url = QUrl::fromUserInput(href);
            }

            // Check for deleted or unsupported items (resp. 404 and 403)
            if (!status.isEmpty()) {
                if (status.contains("404"_L1)) {
                    mDeletedItems.append(url.toDisplayString());
                    continue;
                }
                if (status.contains("403"_L1)) {
                    // RFC 6578 #3.2: unsupported member collections are reported as 403 Forbidden.
                    qCWarning(KDAV_LOG()) << "Unsupported resource: " << url.toDisplayString() << "occurs if resource is an unsupported collection !";
                    continue;
                }
            }

            // For added or changed elements, search for desired properties
            if (!propStat) {
                qCWarning(KDAV_LOG) << "DAV:response element without propstat element - not RFC compliant";
                setError(ERR_MALFORMED_RESPONSE);
                setErrorText(i18n("Invalid server response payload, response item missing PROPSTAT element"));
                emitResult();
                return;
            }

            // Check whether it is a DAV collection, if so skip it
            if (propStat->isCollection) {
                continue;
            }

            const QString itemUrl = url.toDisplayString();
            qCDebug(KDAV_LOG) << href << "->" << itemUrl;
            auto _url = url;
            _url.setUserInfo(mUrl.url().userInfo());

            DavItem item;
            item.setUrl(DavUrl(_url, mUrl.protocol()));
            item.setEtag(propStat->etag);
            mChangedItems << item;
            continue;
        }

        // Unknown property, skip it
        reader.skipCurrentElement();
    }
    emitResult();
}

#include "moc_davitemssyncjob.cpp"
