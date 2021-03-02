/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemdeletejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <KIO/DeleteJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class DavItemDeleteJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);
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
    KIO::DeleteJob *job = KIO::del(d->mItem.url().url(), KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->addMetaData(QStringLiteral("customHTTPHeader"), QStringLiteral("If-Match: ") + d->mItem.etag());
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));

    connect(job, &KIO::DeleteJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
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

void DavItemDeleteJobPrivate::davJobFinished(KJob *job)
{
    KIO::DeleteJob *deleteJob = qobject_cast<KIO::DeleteJob *>(job);

    if (deleteJob->error() && deleteJob->error() != KIO::ERR_NO_CONTENT) {
        const int responseCode = deleteJob->queryMetaData(QStringLiteral("responsecode")).isEmpty() //
            ? 0
            : deleteJob->queryMetaData(QStringLiteral("responsecode")).toInt();

        if (responseCode != 404 && responseCode != 410) {
            setLatestResponseCode(responseCode);
            setError(ERR_ITEMDELETE);
            setJobErrorText(deleteJob->errorText());
            setJobError(deleteJob->error());
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
