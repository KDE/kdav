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

    setLatestResponseCode(responseCode);

    if (storedJob->error()) {
        setLatestResponseCode(responseCode);
        setError(ERR_PROBLEM_WITH_REQUEST);
        setJobErrorText(storedJob->errorText());
        setJobError(storedJob->error());
        setErrorTextFromDavError();
    } else {
        d->mItem.setData(storedJob->data());
        d->mItem.setContentType(storedJob->queryMetaData(QStringLiteral("content-type")));
        d->mItem.setEtag(etagFromHeaders(storedJob->queryMetaData(QStringLiteral("HTTP-Headers"))));
    }

    emitResult();
}
