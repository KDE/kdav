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

#include "davitemcreatejob.h"

#include "davitemfetchjob.h"
#include "davmanager.h"
#include "daverror.h"
#include "davjob.h"

#include "libkdav_debug.h"

using namespace KDAV;

DavItemCreateJob::DavItemCreateJob(const DavItem &item, QObject *parent)
    : DavJobBase(parent), mItem(item), mRedirectCount(0)
{
}

void DavItemCreateJob::start()
{
    auto job = DavManager::self()->createCreateJob(mItem.data(), itemUrl(), mItem.contentType().toLatin1());
    connect(job, &DavJob::result, this, &DavItemCreateJob::davJobFinished);
}

DavItem DavItemCreateJob::item() const
{
    return mItem;
}

QUrl DavItemCreateJob::itemUrl() const
{
    return mItem.url().url();
}


static QUrl assembleUrl(QUrl existingUrl, const QString &location)
{
    if (location.isEmpty()) {
        return existingUrl;
    } else if (location.startsWith(QLatin1Char('/'))) {
        auto url = existingUrl;
        url.setPath(location, QUrl::TolerantMode);
        return url;
    } else {
        return QUrl::fromUserInput(location);
    }
    return {};
}

void DavItemCreateJob::davJobFinished(KJob *job)
{
    qWarning() << "Create job finished";
    auto *storedJob = qobject_cast<DavJob*>(job);
    const int responseCode = storedJob->responseCode();

    if (responseCode == 301 || responseCode == 302 || responseCode == 307 || responseCode == 308) {
        if (mRedirectCount > 4) {
            setLatestResponseCode(responseCode);
            setError(UserDefinedError + responseCode);
            emitResult();
        } else {
            auto url = assembleUrl(storedJob->url(), storedJob->getLocationHeader());
            QUrl _itemUrl(url);
            _itemUrl.setUserInfo(itemUrl().userInfo());
            mItem.setUrl(DavUrl(_itemUrl, mItem.url().protocol()));

            ++mRedirectCount;
            start();
        }

        return;
    }

    if (storedJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_ITEMCREATE);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();

        emitResult();
        return;
    }

    auto url = assembleUrl(storedJob->url(), storedJob->getLocationHeader());
    url.setUserInfo(itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    connect(fetchJob, &DavItemFetchJob::result, this, &DavItemCreateJob::itemRefreshed);
    fetchJob->start();
}

void DavItemCreateJob::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    }
    emitResult();
}

