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
