/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemdeletejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <QNetworkReply>
#include <QNetworkRequest>

using namespace KDAV;

namespace KDAV
{
class DavItemDeleteJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(QNetworkReply *reply);
    void conflictingItemFetched(KJob *job);

    DavItem mItem;
    DavItem mFreshItem;
    int mFreshResponseCode = -1;
};
}

DavItemDeleteJob::DavItemDeleteJob(const DavItem &item, QObject *parent)
    : DavJobBase(new DavItemDeleteJobPrivate, parent)
{
    Q_D(DavItemDeleteJob);
    d->mItem = item;
}

void DavItemDeleteJob::start()
{
    Q_D(DavItemDeleteJob);
    QNetworkRequest request(d->mItem.url().url());
    request.setHeader(QNetworkRequest::IfMatchHeader, d->mItem.etag());
    request.setHeader(QNetworkRequest::UserAgentHeader, DavManager::self()->userAgent());

    QNetworkReply *reply = DavManager::self()->networkAccessManager()->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [d, reply]() {
        d->davJobFinished(reply);
    });
}

DavItem DavItemDeleteJob::freshItem() const
{
    Q_D(const DavItemDeleteJob);
    return d->mFreshItem;
}

int DavItemDeleteJob::freshResponseCode() const
{
    Q_D(const DavItemDeleteJob);
    return d->mFreshResponseCode;
}

void DavItemDeleteJobPrivate::davJobFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        const int responseCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // 404 and 410 mean the item is already gone, treat as success
        if (responseCode != 404 && responseCode != 410) {
            setLatestResponseCode(responseCode);
            setError(ERR_ITEMDELETE);
            setJobErrorText(reply->errorString());
            setJobError(reply->error());
            setErrorTextFromDavError();
        }

        if (q_ptr->hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
            QObject::connect(fetchJob, &DavItemFetchJob::result, q_ptr, [this](KJob *job) {
                conflictingItemFetched(job);
            });
            fetchJob->start();
            return;
        }
    }

    emitResult();
}

void DavItemDeleteJobPrivate::conflictingItemFetched(KJob *job)
{
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        mFreshItem = fetchJob->item();
    }

    emitResult();
}

#include "moc_davitemdeletejob.cpp"
