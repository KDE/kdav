/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionsmultifetchjob.h"

#include "davcollectionsfetchjob.h"

using namespace KDAV;

namespace KDAV {
class DavCollectionsMultiFetchJobPrivate
{
public:
    DavUrl::List mUrls;
    DavCollection::List mCollections;
    int mSubJobCount = -1;
};
}

DavCollectionsMultiFetchJob::DavCollectionsMultiFetchJob(const DavUrl::List &urls, QObject *parent)
    : KJob(parent)
    , d(new DavCollectionsMultiFetchJobPrivate)
{
    d->mUrls = urls;
    d->mSubJobCount = urls.size();
}

DavCollectionsMultiFetchJob::~DavCollectionsMultiFetchJob() = default;

void DavCollectionsMultiFetchJob::start()
{
    if (d->mUrls.isEmpty()) {
        emitResult();
    }

    for (const DavUrl &url : qAsConst(d->mUrls)) {
        DavCollectionsFetchJob *job = new DavCollectionsFetchJob(url, this);
        connect(job, &DavCollectionsFetchJob::result, this, &DavCollectionsMultiFetchJob::davJobFinished);
        connect(job, &DavCollectionsFetchJob::collectionDiscovered, this, &DavCollectionsMultiFetchJob::collectionDiscovered);
        job->start();
    }
}

DavCollection::List DavCollectionsMultiFetchJob::collections() const
{
    return d->mCollections;
}

void DavCollectionsMultiFetchJob::davJobFinished(KJob *job)
{
    DavCollectionsFetchJob *fetchJob = qobject_cast<DavCollectionsFetchJob *>(job);

    if (job->error()) {
        setError(job->error());
        setErrorText(job->errorText());
    } else {
        d->mCollections << fetchJob->collections();
    }

    if (--d->mSubJobCount == 0) {
        emitResult();
    }
}
