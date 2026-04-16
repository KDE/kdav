// SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectioncreatejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"
#include "davxml_p.h"

#include <QColor>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamWriter>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{
class DavCollectionCreateJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);

    DavCollection mCollection;
    int mRedirectCount = 0;

    Q_DECLARE_PUBLIC(DavCollectionCreateJob)
};
}

DavCollectionCreateJob::DavCollectionCreateJob(const DavCollection &collection, QObject *parent)
    : DavJobBase(new DavCollectionCreateJobPrivate, parent)
{
    Q_D(DavCollectionCreateJob);
    d->mCollection = collection;
}

void DavCollectionCreateJob::start()
{
    Q_D(DavCollectionCreateJob);

    QString output;
    QXmlStreamWriter writer(&output);
    writer.setAutoFormatting(true);
    Xml::writeMkCol(writer, d->mCollection);

    QNetworkRequest request(collectionUrl());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->sendCustomRequest(request, "MKCOL", output.toUtf8());
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavCollection DavCollectionCreateJob::collection() const
{
    Q_D(const DavCollectionCreateJob);
    return d->mCollection;
}

QUrl DavCollectionCreateJob::collectionUrl() const
{
    Q_D(const DavCollectionCreateJob);
    return d->mCollection.url().url();
}

void DavCollectionCreateJobPrivate::davJobFinished(QNetworkReply *reply)
{
    Q_Q(DavCollectionCreateJob);
    reply->deleteLater();

    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        setLatestResponseCode(responseCode);
        setError(ERR_ITEMCREATE);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();

        emitResult();
        return;
    }

    // The 'Location:' HTTP header is used to indicate the new URL
    const QString location = reply->header(QNetworkRequest::LocationHeader).toString();

    QUrl url;
    if (location.isEmpty()) {
        url = reply->url();
    } else if (location.startsWith(QLatin1Char('/'))) {
        url = reply->url();
        url.setPath(location, QUrl::TolerantMode);
    } else {
        url = QUrl::fromUserInput(location);
    }

    if (responseCode == 301 || responseCode == 302 || responseCode == 307 || responseCode == 308) {
        if (mRedirectCount > 4) {
            setLatestResponseCode(responseCode);
            setError(DavCollectionCreateJob::UserDefinedError + responseCode);
            emitResult();
        } else {
            QUrl _collectionUrl(url);
            _collectionUrl.setUserInfo(q->collectionUrl().userInfo());
            mCollection.setUrl(DavUrl(_collectionUrl, mCollection.url().protocol()));

            ++mRedirectCount;
            q->start();
        }

        return;
    }

    url.setUserInfo(q->collectionUrl().userInfo());
    mCollection.setUrl(DavUrl(url, mCollection.url().protocol()));
    emitResult();
}

#include "moc_davcollectioncreatejob.cpp"
