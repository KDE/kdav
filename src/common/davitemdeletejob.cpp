/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "davitemdeletejob.h"
#include "davjobbase_p.h"

#include "davitemfetchjob.h"
#include "davmanager_p.h"
#include "daverror.h"

#include <KIO/DeleteJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV {
class DavItemDeleteJobPrivate : public DavJobBasePrivate
{
public:
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

    connect(job, &KIO::DeleteJob::result, this, &DavItemDeleteJob::davJobFinished);
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

void DavItemDeleteJob::davJobFinished(KJob *job)
{
    Q_D(DavItemDeleteJob);
    KIO::DeleteJob *deleteJob = qobject_cast<KIO::DeleteJob *>(job);

    if (deleteJob->error() && deleteJob->error() != KIO::ERR_NO_CONTENT) {
        const int responseCode = deleteJob->queryMetaData(QStringLiteral("responsecode")).isEmpty()
                                 ? 0
                                 : deleteJob->queryMetaData(QStringLiteral("responsecode")).toInt();

        if (responseCode != 404 && responseCode != 410) {
            setLatestResponseCode(responseCode);
            setError(ERR_ITEMDELETE);
            setJobErrorText(deleteJob->errorText());
            setJobError(deleteJob->error());
            setErrorTextFromDavError();
        }

        if (hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(d->mItem);
            connect(fetchJob, &DavItemFetchJob::result, this, &DavItemDeleteJob::conflictingItemFetched);
            fetchJob->start();
            return;
        }
    }

    emitResult();
}

void DavItemDeleteJob::conflictingItemFetched(KJob *job)
{
    Q_D(DavItemDeleteJob);
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    d->mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        d->mFreshItem = fetchJob->item();
    }

    emitResult();
}
