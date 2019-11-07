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

#include "davitemmodifyjob.h"
#include "davjobbase_p.h"

#include "davitemfetchjob.h"
#include "davmanager_p.h"
#include "daverror.h"

#include <KIO/Job>

using namespace KDAV;
namespace KDAV {
class DavItemModifyJobPrivate : public DavJobBasePrivate
{
public:
    DavItem mItem;
    DavItem mFreshItem;
    int mFreshResponseCode = 0;
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
    QString headers = QStringLiteral("Content-Type: ");
    headers += d->mItem.contentType();
    headers += QLatin1String("\r\n");
    headers += QLatin1String("If-Match: ") + d->mItem.etag();

    KIO::StoredTransferJob *job = KIO::storedPut(d->mItem.data(), itemUrl(), -1, KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->addMetaData(QStringLiteral("customHTTPHeader"), headers);
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));

    connect(job, &KIO::StoredTransferJob::result, this, &DavItemModifyJob::davJobFinished);
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

void DavItemModifyJob::davJobFinished(KJob *job)
{
    Q_D(DavItemModifyJob);
    KIO::StoredTransferJob *storedJob = qobject_cast<KIO::StoredTransferJob *>(job);

    if (storedJob->error()) {
        const int responseCode = storedJob->queryMetaData(QStringLiteral("responsecode")).isEmpty()
                                 ? 0
                                 : storedJob->queryMetaData(QStringLiteral("responsecode")).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_ITEMMODIFY);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();

        if (hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(d->mItem);
            connect(fetchJob, &DavItemFetchJob::result, this, &DavItemModifyJob::conflictingItemFetched);
            fetchJob->start();
        } else {
            emitResult();
        }

        return;
    }

    // The 'Location:' HTTP header is used to indicate the new URL
    const QStringList allHeaders = storedJob->queryMetaData(QStringLiteral("HTTP-Headers")).split(QLatin1Char('\n'));
    QString location;
    for (const QString &header : allHeaders) {
        if (header.startsWith(QLatin1String("location:"), Qt::CaseInsensitive)) {
            location = header.section(QLatin1Char(' '), 1);
        }
    }

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
    d->mItem.setUrl(DavUrl(url, d->mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(d->mItem);
    connect(fetchJob, &DavItemFetchJob::result, this, &DavItemModifyJob::itemRefreshed);
    fetchJob->start();
}

void DavItemModifyJob::itemRefreshed(KJob *job)
{
    Q_D(DavItemModifyJob);
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        d->mItem.setEtag(fetchJob->item().etag());
    } else {
        d->mItem.setEtag(QString());
    }
    emitResult();
}

void DavItemModifyJob::conflictingItemFetched(KJob *job)
{
    Q_D(DavItemModifyJob);
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    d->mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        d->mFreshItem = fetchJob->item();
    }

    emitResult();
}
