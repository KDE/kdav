/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemfetchjob.h"
#include "davjobbase_p.h"

#include "davmanager_p.h"
#include "daverror.h"

#include <KIO/DavJob>
#include <KIO/Job>

using namespace KDAV;
namespace KDAV {
class DavItemFetchJobPrivate : public DavJobBasePrivate
{
public:
    DavUrl mUrl;
    DavItem mItem;
};
}

static QString etagFromHeaders(const QString &headers)
{
    const QStringList allHeaders = headers.split(QLatin1Char('\n'));

    QString etag;
    for (const QString &header : allHeaders) {
        if (header.startsWith(QLatin1String("etag:"), Qt::CaseInsensitive)) {
            etag = header.section(QLatin1Char(' '), 1);
        }
    }

    return etag;
}

DavItemFetchJob::DavItemFetchJob(const DavItem &item, QObject *parent)
    : DavJobBase(new DavItemFetchJobPrivate, parent)
{
    Q_D(DavItemFetchJob);
    d->mItem = item;
}

void DavItemFetchJob::start()
{
    Q_D(DavItemFetchJob);
    KIO::StoredTransferJob *job = KIO::storedGet(d->mItem.url().url(), KIO::Reload, KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    // Work around a strange bug in Zimbra (seen at least on CE 5.0.18) : if the user-agent
    // contains "Mozilla", some strange debug data is displayed in the shared calendars.
    // This kinda mess up the events parsing...
    job->addMetaData(QStringLiteral("UserAgent"), QStringLiteral("KDE DAV groupware client"));
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));

    connect(job, &KIO::StoredTransferJob::result, this, &DavItemFetchJob::davJobFinished);
}

DavItem DavItemFetchJob::item() const
{
    Q_D(const DavItemFetchJob);
    return d->mItem;
}

void DavItemFetchJob::davJobFinished(KJob *job)
{
    Q_D(DavItemFetchJob);
    KIO::StoredTransferJob *storedJob = qobject_cast<KIO::StoredTransferJob *>(job);
    const QString responseCodeStr = storedJob->queryMetaData(QStringLiteral("responsecode"));
    const int responseCode = responseCodeStr.isEmpty()
                             ? 0
                             : responseCodeStr.toInt();

    d->setLatestResponseCode(responseCode);

    if (storedJob->error()) {
        d->setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        d->setJobErrorText(storedJob->errorText());
        d->setJobError(storedJob->error());
        d->setErrorTextFromDavError();
    } else {
        d->mItem.setData(storedJob->data());
        d->mItem.setContentType(storedJob->queryMetaData(QStringLiteral("content-type")));
        d->mItem.setEtag(etagFromHeaders(storedJob->queryMetaData(QStringLiteral("HTTP-Headers"))));
    }

    emitResult();
}
