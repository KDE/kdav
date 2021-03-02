/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemcreatejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"
#include "davmanager_p.h"

#include <KIO/DavJob>
#include <KIO/Job>

#include "libkdav_debug.h"

using namespace KDAV;

namespace KDAV
{
class DavItemCreateJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);
    void itemRefreshed(KJob *job);

    DavItem mItem;
    int mRedirectCount = 0;

    Q_DECLARE_PUBLIC(DavItemCreateJob)
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

    connect(job, &KIO::StoredTransferJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
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

void DavItemCreateJobPrivate::davJobFinished(KJob *job)
{
    Q_Q(DavItemCreateJob);
    KIO::StoredTransferJob *storedJob = qobject_cast<KIO::StoredTransferJob *>(job);
    const QString responseCodeStr = storedJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty() ? 0 : responseCodeStr.toInt();
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
        if (mRedirectCount > 4) {
            setLatestResponseCode(responseCode);
            setError(DavItemCreateJob::UserDefinedError + responseCode);
            emitResult();
        } else {
            QUrl _itemUrl(url);
            _itemUrl.setUserInfo(q->itemUrl().userInfo());
            mItem.setUrl(DavUrl(_itemUrl, mItem.url().protocol()));

            ++mRedirectCount;
            q->start();
        }

        return;
    }

    url.setUserInfo(q->itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
        itemRefreshed(job);
    });
    fetchJob->start();
}

void DavItemCreateJobPrivate::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    }
    emitResult();
}
