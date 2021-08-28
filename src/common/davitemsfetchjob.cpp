/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    Based on DavItemsListJob:
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemsfetchjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davmultigetprotocol_p.h"
#include "utils_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class DavItemsFetchJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);

    DavUrl mCollectionUrl;
    QStringList mUrls;
    QMap<QString, DavItem> mItems;
};
}

DavItemsFetchJob::DavItemsFetchJob(const DavUrl &collectionUrl, const QStringList &urls, QObject *parent)
    : DavJobBase(new DavItemsFetchJobPrivate, parent)
{
    Q_D(DavItemsFetchJob);
    d->mCollectionUrl = collectionUrl;
    d->mUrls = urls;
}

void DavItemsFetchJob::start()
{
    Q_D(DavItemsFetchJob);
    const DavMultigetProtocol *protocol = dynamic_cast<const DavMultigetProtocol *>(DavManager::davProtocol(d->mCollectionUrl.protocol()));
    if (!protocol) {
        setError(ERR_NO_MULTIGET);
        d->setErrorTextFromDavError();
        emitResult();
        return;
    }

    const QDomDocument report = protocol->itemsReportQuery(d->mUrls)->buildQuery();
    KIO::DavJob *job = DavManager::self()->createReportJob(d->mCollectionUrl.url(), report.toString(), QStringLiteral("0"));
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    connect(job, &KIO::DavJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
}

DavItem::List DavItemsFetchJob::items() const
{
    Q_D(const DavItemsFetchJob);
    DavItem::List values;
    values.reserve(d->mItems.size());
    for (const auto &value : std::as_const(d->mItems)) {
        values << value;
    }
    return values;
}

DavItem DavItemsFetchJob::item(const QString &url) const
{
    Q_D(const DavItemsFetchJob);
    return d->mItems.value(url);
}

void DavItemsFetchJobPrivate::davJobFinished(KJob *job)
{
    KIO::DavJob *davJob = qobject_cast<KIO::DavJob *>(job);
    const QString responseCodeStr = davJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();

    // KIO::DavJob does not set error() even if the HTTP status code is a 4xx or a 5xx
    if (davJob->error() || (responseCode >= 400 && responseCode < 600)) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(davJob->errorText());
        setJobError(davJob->error());
        setErrorTextFromDavError();

        emitResult();
        return;
    }

    const DavMultigetProtocol *protocol = static_cast<const DavMultigetProtocol *>(DavManager::davProtocol(mCollectionUrl.protocol()));

    QDomDocument document;
    document.setContent(davJob->responseData(), true);
    const QDomElement documentElement = document.documentElement();

    QDomElement responseElement = Utils::firstChildElementNS(documentElement, QStringLiteral("DAV:"), QStringLiteral("response"));

    while (!responseElement.isNull()) {
        QDomElement propstatElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("propstat"));

        if (propstatElement.isNull()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        // Check for errors
        const QDomElement statusElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("status"));
        if (!statusElement.text().contains(QLatin1String("200"))) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));

        DavItem item;

        // extract path
        const QDomElement hrefElement = Utils::firstChildElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("href"));
        const QString href = hrefElement.text();

        QUrl url = davJob->url();
        if (href.startsWith(QLatin1Char('/'))) {
            // href is only a path, use request url to complete
            url.setPath(href, QUrl::TolerantMode);
        } else {
            // href is a complete url
            url = QUrl::fromUserInput(href);
        }

        auto _url = url;
        _url.setUserInfo(mCollectionUrl.url().userInfo());
        item.setUrl(DavUrl(_url, mCollectionUrl.protocol()));

        // extract ETag
        const QDomElement getetagElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("getetag"));
        item.setEtag(getetagElement.text());

        // extract content
        const QDomElement dataElement = Utils::firstChildElementNS(propElement, protocol->responseNamespace(), protocol->dataTagName());

        if (dataElement.isNull()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        const QByteArray data = dataElement.firstChild().toText().data().toUtf8();
        if (data.isEmpty()) {
            responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
            continue;
        }

        item.setData(data);

        mItems.insert(item.url().toDisplayString(), item);
        responseElement = Utils::nextSiblingElementNS(responseElement, QStringLiteral("DAV:"), QStringLiteral("response"));
    }

    emitResult();
}
