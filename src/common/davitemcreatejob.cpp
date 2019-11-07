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

#include "davitemcreatejob.h"
#include "davjobbase_p.h"

#include "davitemfetchjob.h"
#include "davmanager_p.h"
#include "daverror.h"

#include <KIO/DavJob>
#include <KIO/Job>

#include "libkdav_debug.h"

using namespace KDAV;

namespace KDAV {
class DavItemCreateJobPrivate : public DavJobBasePrivate
{
public:
    DavItem mItem;
    int mRedirectCount = 0;
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
    QString headers = QStringLiteral("Content-Type: ");
    headers += d->mItem.contentType();
    headers += QLatin1String("\r\n");
    headers += QLatin1String("If-None-Match: *");

    KIO::StoredTransferJob *job = KIO::storedPut(d->mItem.data(), itemUrl(), -1, KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->addMetaData(QStringLiteral("customHTTPHeader"), headers);
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    job->setRedirectionHandlingEnabled(false);

    connect(job, &KIO::StoredTransferJob::result, this, &DavItemCreateJob::davJobFinished);
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

void DavItemCreateJob::davJobFinished(KJob *job)
{
    Q_D(DavItemCreateJob);
    KIO::StoredTransferJob *storedJob = qobject_cast<KIO::StoredTransferJob *>(job);
    const QString responseCodeStr = storedJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty()
                             ? 0
                             : responseCodeStr.toInt();
    if (storedJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_ITEMCREATE);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();

        emitResult();
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

    if (responseCode == 301 || responseCode == 302 || responseCode == 307 || responseCode == 308) {
        if (d->mRedirectCount > 4) {
            setLatestResponseCode(responseCode);
            setError(UserDefinedError + responseCode);
            emitResult();
        } else {
            QUrl _itemUrl(url);
            _itemUrl.setUserInfo(itemUrl().userInfo());
            d->mItem.setUrl(DavUrl(_itemUrl, d->mItem.url().protocol()));

            ++d->mRedirectCount;
            start();
        }

        return;
    }

    url.setUserInfo(itemUrl().userInfo());
    d->mItem.setUrl(DavUrl(url, d->mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(d->mItem);
    connect(fetchJob, &DavItemFetchJob::result, this, &DavItemCreateJob::itemRefreshed);
    fetchJob->start();
}

void DavItemCreateJob::itemRefreshed(KJob *job)
{
    Q_D(DavItemCreateJob);
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        d->mItem.setEtag(fetchJob->item().etag());
    }
    emitResult();
}
