/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemcreatejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{
class DavItemCreateJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);
    void itemRefreshed(KJob *job);

    DavItem mItem;
    int mRedirectCount = 0;

    Q_DECLARE_PUBLIC(DavItemCreateJob)
};
}

DavItemCreateJob::DavItemCreateJob(const DavItem &item, QObject *parent)
    : DavJobBase(new DavItemCreateJobPrivate, parent)
{
    Q_D(DavItemCreateJob);
    d->mItem = item;
}

void DavItemCreateJob::start()
{
    Q_D(DavItemCreateJob);
    QNetworkRequest request(itemUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, d->mItem.contentType());
    request.setHeader(QNetworkRequest::IfNoneMatchHeader, u"*"_s);
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->put(request, d->mItem.data());
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavItem DavItemCreateJob::item() const
{
    Q_D(const DavItemCreateJob);
    return d->mItem;
}

QUrl DavItemCreateJob::itemUrl() const
{
    Q_D(const DavItemCreateJob);
    return d->mItem.url().url();
}

void DavItemCreateJobPrivate::davJobFinished(QNetworkReply *reply)
{
    Q_Q(DavItemCreateJob);
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
            setError(DavItemCreateJob::UserDefinedError + responseCode);
            emitResult();
        } else {
            QUrl _itemUrl(url);
            _itemUrl.setUserInfo(q->itemUrl().userInfo());
            mItem.setUrl(DavUrl(_itemUrl, mItem.url().protocol()));

            ++mRedirectCount;
            q->start();
        }

        return;
    }

    url.setUserInfo(q->itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
        itemRefreshed(job);
    });
    fetchJob->start();
}

void DavItemCreateJobPrivate::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    }
    emitResult();
}

#include "moc_davitemcreatejob.cpp"
