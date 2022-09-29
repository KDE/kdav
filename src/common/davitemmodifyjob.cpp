/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemmodifyjob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davitemfetchjob.h"

#include <KIO/Job>

using namespace KDAV;
namespace KDAV
{
class DavItemModifyJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);
    void itemRefreshed(KJob *job);
    void conflictingItemFetched(KJob *job);

    DavItem mItem;
    DavItem mFreshItem;
    int mFreshResponseCode = 0;

    Q_DECLARE_PUBLIC(DavItemModifyJob)
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

    connect(job, &KIO::StoredTransferJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
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

void DavItemModifyJobPrivate::davJobFinished(KJob *job)
{
    Q_Q(DavItemModifyJob);
    KIO::StoredTransferJob *storedJob = qobject_cast<KIO::StoredTransferJob *>(job);

    if (storedJob->error()) {
        const int responseCode = storedJob->queryMetaData(QStringLiteral("responsecode")).isEmpty() //
            ? 0
            : storedJob->queryMetaData(QStringLiteral("responsecode")).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_ITEMMODIFY);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();

        if (q->hasConflict()) {
            DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
            QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
                conflictingItemFetched(job);
            });
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

    url.setUserInfo(q->itemUrl().userInfo());
    mItem.setUrl(DavUrl(url, mItem.url().protocol()));

    DavItemFetchJob *fetchJob = new DavItemFetchJob(mItem);
    QObject::connect(fetchJob, &DavItemFetchJob::result, q, [this](KJob *job) {
        itemRefreshed(job);
    });
    fetchJob->start();
}

void DavItemModifyJobPrivate::itemRefreshed(KJob *job)
{
    if (!job->error()) {
        DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
        mItem.setEtag(fetchJob->item().etag());
    } else {
        mItem.setEtag(QString());
    }
    emitResult();
}

void DavItemModifyJobPrivate::conflictingItemFetched(KJob *job)
{
    DavItemFetchJob *fetchJob = qobject_cast<DavItemFetchJob *>(job);
    mFreshResponseCode = fetchJob->latestResponseCode();

    if (!job->error()) {
        mFreshItem = fetchJob->item();
    }

    emitResult();
}
