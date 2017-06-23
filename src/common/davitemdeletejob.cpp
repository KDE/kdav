/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "davitemdeletejob.h"

#include "davitemfetchjob.h"
#include "davmanager.h"
#include "daverror.h"
#include "davjob.h"

using namespace KDAV;

DavItemDeleteJob::DavItemDeleteJob(const DavItem &item, QObject *parent)
    : DavJobBase(parent), mItem(item), mFreshResponseCode(-1)
{
}

void DavItemDeleteJob::start()
{
    DavJob *job = DavManager::self()->createDeleteJob(mItem.url().url());
    connect(job, &DavJob::result, this, &DavItemDeleteJob::davJobFinished);
}

DavItem DavItemDeleteJob::freshItem() const
{
    return mFreshItem;
}

int DavItemDeleteJob::freshResponseCode() const
{
    return mFreshResponseCode;
}

void DavItemDeleteJob::davJobFinished(KJob *job)
{
    auto *deleteJob = qobject_cast<DavJob *>(job);

    if (deleteJob->error()) {
        const int responseCode = deleteJob->responseCode();

        if (responseCode != 404 && responseCode != 410) {
            setLatestResponseCode(responseCode);
            setError(ERR_ITEMDELETE);
            setJobErrorText(deleteJob->errorText());
            setJobError(deleteJob->error());
            setErrorTextFromDavError();
        }

        if (hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
            connect(fetchJob, &DavItemFetchJob::result, this, &DavItemDeleteJob::conflictingItemFetched);
            fetchJob->start();
            return;
        }
    }

    emitResult();
}

void DavItemDeleteJob::conflictingItemFetched(KJob *job)
{
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        mFreshItem = fetchJob->item();
    }

    emitResult();
}

