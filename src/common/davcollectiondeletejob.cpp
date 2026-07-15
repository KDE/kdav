/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectiondeletejob.h"
#include "davjobbase_p.h"

#include "daverror.h"
#include "davpushdontnotify.h"

#include <KIO/DeleteJob>
#include <KIO/Job>

using namespace KDAV;

namespace KDAV
{
class DavCollectionDeleteJobPrivate : public DavJobBasePrivate
{
public:
    void davJobFinished(KJob *job);

    DavUrl mUrl;

    DavPushDontNotify mPushDontNotify;
};
}

DavCollectionDeleteJob::DavCollectionDeleteJob(const DavUrl &url, QObject *parent)
    : DavJobBase(new DavCollectionDeleteJobPrivate, parent)
{
    Q_D(DavCollectionDeleteJob);
    d->mUrl = url;
}

void DavCollectionDeleteJob::setPushDontNotify(const DavPushDontNotify &dontNotify)
{
    Q_D(DavCollectionDeleteJob);
    d->mPushDontNotify = dontNotify;
}

DavPushDontNotify DavCollectionDeleteJob::pushDontNotify() const
{
    Q_D(const DavCollectionDeleteJob);
    return d->mPushDontNotify;
}

void DavCollectionDeleteJob::start()
{
    Q_D(DavCollectionDeleteJob);
    KIO::DeleteJob *job = KIO::del(d->mUrl.url(), KIO::HideProgressInfo | KIO::DefaultFlags);
    job->addMetaData(QStringLiteral("PropagateHttpHeader"), QStringLiteral("true"));
    job->addMetaData(QStringLiteral("cookies"), QStringLiteral("none"));
    job->addMetaData(QStringLiteral("no-auth-prompt"), QStringLiteral("true"));
    if (!d->mPushDontNotify.isNull()) {
        job->addMetaData(QStringLiteral("customHTTPHeader"), d->mPushDontNotify.davHeader());
    }

    connect(job, &KIO::DeleteJob::result, this, [d](KJob *job) {
        d->davJobFinished(job);
    });
}

void DavCollectionDeleteJobPrivate::davJobFinished(KJob *job)
{
    KIO::DeleteJob *deleteJob = qobject_cast<KIO::DeleteJob *>(job);

    if (deleteJob->error() && deleteJob->error() != KIO::ERR_NO_CONTENT) {
        const int responseCode = deleteJob->queryMetaData(QStringLiteral("responsecode")).isEmpty() //
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

#include "moc_davcollectiondeletejob.cpp"
