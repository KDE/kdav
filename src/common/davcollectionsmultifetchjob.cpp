/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionsmultifetchjob.h"

#include "davcollectionsfetchjob.h"

using namespace KDAV;

namespace KDAV
{
class DavCollectionsMultiFetchJobPrivate
{
public:
    DavCollection::List mCollections;
};
}

DavCollectionsMultiFetchJob::DavCollectionsMultiFetchJob(const DavUrl::List &urls, QObject *parent)
    : KCompositeJob(parent)
    , d(new DavCollectionsMultiFetchJobPrivate)
{
    for (const DavUrl &url : std::as_const(urls)) {
        DavCollectionsFetchJob *job = new DavCollectionsFetchJob(url, this);
        connect(job, &DavCollectionsFetchJob::collectionDiscovered, this, &DavCollectionsMultiFetchJob::collectionDiscovered);
        addSubjob(job);
    }
}

DavCollectionsMultiFetchJob::~DavCollectionsMultiFetchJob() = default;

void DavCollectionsMultiFetchJob::start()
{
    if (!hasSubjobs()) {
        emitResult();
    } else {
        for (KJob *job : subjobs()) {
            job->start();
        }
    }
}

DavCollection::List DavCollectionsMultiFetchJob::collections() const
{
    return d->mCollections;
}

void DavCollectionsMultiFetchJob::slotResult(KJob *job)
{
    // If we use KCompositeJob::slotResult(job) we end up with behaviour that's very
    // hard to unittest: the valid URLs might or might not get processed.
    // Let's wait until all subjobs are done before emitting result.

    if (job->error() && !error()) {
        // Store error only if first error
        setError(job->error());
        setErrorText(job->errorText());
    }
    if (!job->error()) {
        DavCollectionsFetchJob *fetchJob = qobject_cast<DavCollectionsFetchJob *>(job);
        d->mCollections << fetchJob->collections();
    }
    removeSubjob(job);
    if (!hasSubjobs()) {
        emitResult();
    }
}
