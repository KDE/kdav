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

#ifndef KDAV2_DAVCOLLECTIONSFETCHJOB_H
#define KDAV2_DAVCOLLECTIONSFETCHJOB_H

#include "kpimkdav2_export.h"

#include "davcollection.h"
#include "davjobbase.h"
#include "davurl.h"

#include <KCoreAddons/KJob>

namespace KDAV2
{

/**
 * @short A job that fetches all DAV collection.
 *
 * This job is used to fetch all DAV collection that are available
 * under a certain DAV url.
 */
class KPIMKDAV2_EXPORT DavCollectionsFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav collections fetch job.
     *
     * @param url The DAV url of the DAV collection whose sub collections shall be fetched.
     * @param parent The parent object.
     */
    explicit DavCollectionsFetchJob(const DavUrl &url, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() Q_DECL_OVERRIDE;

    /**
     * Returns the list of fetched DAV collections.
     */
    DavCollection::List collections() const;

    /**
     * Return the DavUrl used by this job
     */
    DavUrl davUrl() const;

Q_SIGNALS:
    /**
     * This signal is emitted every time a new collection has been discovered.
     *
     * @param collectionUrl The URL of the discovered collection
     * @param configuredUrl The URL given to the job
     */
    void collectionDiscovered(int protocol, const QString &collectionUrl, const QString &configuredUrl);

private Q_SLOTS:
    void principalFetchFinished(KJob *);
    void collectionsFetchFinished(KJob *);

private:
    void doCollectionsFetch(const QUrl &url);
    void subjobFinished();

    DavUrl mUrl;
    DavCollection::List mCollections;
    uint mSubJobCount;
};

}

#endif
