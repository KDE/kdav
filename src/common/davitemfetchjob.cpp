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

#include "davitemfetchjob.h"

#include "davmanager.h"
#include "daverror.h"
#include "davjob.h"

using namespace KDAV2;

DavItemFetchJob::DavItemFetchJob(const DavItem &item, QObject *parent)
    : DavJobBase(parent), mItem(item)
{
}

void DavItemFetchJob::start()
{
    auto job = DavManager::self()->createGetJob(mItem.url().url());
    connect(job, &DavJob::result, this, &DavItemFetchJob::davJobFinished);
}

DavItem DavItemFetchJob::item() const
{
    return mItem;
}

void DavItemFetchJob::davJobFinished(KJob *job)
{
    auto *storedJob = qobject_cast<DavJob*>(job);
    const int responseCode = storedJob->responseCode();
    setLatestResponseCode(responseCode);

    if (storedJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();
    } else {
        mItem.setData(storedJob->data());
        mItem.setContentType(storedJob->getContentTypeHeader());
        mItem.setEtag(storedJob->getETagHeader());
    }

    emitResult();
}

