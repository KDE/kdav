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

#include "davitemmodifyjob.h"

#include "davitemfetchjob.h"
#include "davmanager.h"
#include "daverror.h"
#include "davjob.h"

using namespace KDAV2;

DavItemModifyJob::DavItemModifyJob(const DavItem &item, QObject *parent)
    : DavJobBase(parent), mItem(item), mFreshResponseCode(0)
{
}

void DavItemModifyJob::start()
{
    auto job = DavManager::self()->createModifyJob(mItem.data(), itemUrl(), mItem.contentType().toUtf8(), mItem.etag().toUtf8());
    connect(job, &DavJob::result, this, &DavItemModifyJob::davJobFinished);
}

DavItem DavItemModifyJob::item() const
{
    return mItem;
}

DavItem DavItemModifyJob::freshItem() const
{
    return mFreshItem;
}

int DavItemModifyJob::freshResponseCode() const
{
    return mFreshResponseCode;
}

QUrl DavItemModifyJob::itemUrl() const
{
    return mItem.url().url();
}

void DavItemModifyJob::davJobFinished(KJob *job)
{
    auto *storedJob = qobject_cast<DavJob*>(job);

    if (storedJob->error()) {
        const int responseCode = storedJob->responseCode();;

        setLatestResponseCode(responseCode);
        setError(ERR_ITEMMODIFY);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();

        if (hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
            connect(fetchJob, &DavItemFetchJob::result, this, &DavItemModifyJob::conflictingItemFetched);
            fetchJob->start();
        } else {
            emitResult();
        }

        return;
    }

    const auto location = storedJob->getLocationHeader();
    QUrl url;
    if (location.isEmpty()) {
        url = storedJob->url();
    } else if (location.startsWith(QLatin1Char('/'))) {
        url = storedJob->url();
        url.setPath(location, QUrl::TolerantMode);
    } else {
        url = QUrl::fromUserInput(location);
    }

    url.setUserInfo(itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    connect(fetchJob, &DavItemFetchJob::result, this, &DavItemModifyJob::itemRefreshed);
    fetchJob->start();
}

void DavItemModifyJob::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    } else {
        mItem.setEtag(QString());
    }
    emitResult();
}

void DavItemModifyJob::conflictingItemFetched(KJob *job)
{
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        mFreshItem = fetchJob->item();
    }

    emitResult();
}

