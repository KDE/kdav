/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemmodifyjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;

namespace KDAV
{
class DavItemModifyJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);
    void itemRefreshed(KJob *job);
    void conflictingItemFetched(KJob *job);

    DavItem mItem;
    DavItem mFreshItem;
    int mFreshResponseCode = 0;

    Q_DECLARE_PUBLIC(DavItemModifyJob)
};
}

DavItemModifyJob::DavItemModifyJob(const DavItem &item, QObject *parent)
    : DavJobBase(new DavItemModifyJobPrivate, parent)
{
    Q_D(DavItemModifyJob);
    d->mItem = item;
}

void DavItemModifyJob::start()
{
    Q_D(DavItemModifyJob);
    QNetworkRequest request(itemUrl());
    request.setHeader(QNetworkRequest::ContentTypeHeader, d->mItem.contentType());
    request.setHeader(QNetworkRequest::IfMatchHeader, d->mItem.etag());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->put(request, d->mItem.data());
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavItem DavItemModifyJob::item() const
{
    Q_D(const DavItemModifyJob);
    return d->mItem;
}

DavItem DavItemModifyJob::freshItem() const
{
    Q_D(const DavItemModifyJob);
    return d->mFreshItem;
}

int DavItemModifyJob::freshResponseCode() const
{
    Q_D(const DavItemModifyJob);
    return d->mFreshResponseCode;
}

QUrl DavItemModifyJob::itemUrl() const
{
    Q_D(const DavItemModifyJob);
    return d->mItem.url().url();
}

void DavItemModifyJobPrivate::davJobFinished(QNetworkReply *reply)
{
    Q_Q(DavItemModifyJob);
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_ITEMMODIFY);
        setJobErrorText(reply->errorString());
        setJobError(reply->error());
        setErrorTextFromDavError();

        if (q->hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
            QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
                conflictingItemFetched(job);
            });
            fetchJob->start();
        } else {
            emitResult();
        }

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

    url.setUserInfo(q->itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
        itemRefreshed(job);
    });
    fetchJob->start();
}

void DavItemModifyJobPrivate::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    } else {
        mItem.setEtag(QString());
    }
    emitResult();
}

void DavItemModifyJobPrivate::conflictingItemFetched(KJob *job)
{
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        mFreshItem = fetchJob->item();
    }

    emitResult();
}

#include "moc_davitemmodifyjob.cpp"
