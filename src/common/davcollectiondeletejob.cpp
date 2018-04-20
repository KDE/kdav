/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>

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

#include "davcollectiondeletejob.h"

#include "daverror.h"

#include <KIO/DeleteJob>
#include <KIO/Job>

using namespace KDAV;

DavCollectionDeleteJob::DavCollectionDeleteJob(const DavUrl &url, QObject *parent)
    : DavJobBase(parent)
    , mUrl(url)
{
}

void DavCollectionDeleteJob::start()
{
    KIO::DeleteJob *job = KIO::del(mUrl.url(), KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));

    connect(job, &KIO::DeleteJob::result, this, &DavCollectionDeleteJob::davJobFinished);
}

void DavCollectionDeleteJob::davJobFinished(KJob *job)
{
    KIO::DeleteJob *deleteJob = qobject_cast<KIO::DeleteJob *>(job);

    if (deleteJob->error() && deleteJob->error() != KIO::ERR_NO_CONTENT) {
        const int responseCode = deleteJob->queryMetaData(QStringLiteral("responsecode")).isEmpty()
                                 ? 0
                                 : deleteJob->queryMetaData(QStringLiteral("responsecode")).toInt();

        setLatestResponseCode(responseCode);
        setError(ERR_COLLECTIONDELETE);
        setJobErrorText(deleteJob->errorText());
        setJobError(deleteJob->error());
        setErrorTextFromDavError();
    }

    emitResult();
}
