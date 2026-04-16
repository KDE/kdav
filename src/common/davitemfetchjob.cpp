/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemfetchjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;
namespace KDAV
{
class DavItemFetchJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);

    DavUrl mUrl;
    DavItem mItem;
};
}

DavItemFetchJob::DavItemFetchJob(const DavItem &item, QObject *parent)
    : DavJobBase(new DavItemFetchJobPrivate, parent)
{
    Q_D(DavItemFetchJob);
    d->mItem = item;
}

void DavItemFetchJob::start()
{
    Q_D(DavItemFetchJob);
    QNetworkRequest request(d->mItem.url().url());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->get(request);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavItem DavItemFetchJob::item() const
{
    Q_D(const DavItemFetchJob);
    return d->mItem;
}

void DavItemFetchJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    setLatestResponseCode(responseCode);

    if (reply->error() != QNetworkReply::NoError) {
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();
    } else {
        mItem.setData(reply->readAll());
        // Strip optional parameters like "; charset=utf-8" from the content type
        const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString().section(QLatin1Char(';'), 0, 0).trimmed();
        mItem.setContentType(contentType);
        // reply->header(QNetworkRequest::ETagHeader) returns invalid in the unit test
        mItem.setEtag(QString::fromUtf8(reply->rawHeader("etag")));
    }

    emitResult();
}

#include "moc_davitemfetchjob.cpp"
