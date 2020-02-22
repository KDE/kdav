/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTIONSFETCHJOB_H
#define KDAV_DAVCOLLECTIONSFETCHJOB_H

#include "kdav_export.h"

#include "davcollection.h"
#include "davjobbase.h"
#include "davurl.h"

#include <KJob>

namespace KDAV {
class DavCollectionsFetchJobPrivate;

/**
 * @short A job that fetches all DAV collection.
 *
 * This job is used to fetch all DAV collection that are available
 * under a certain DAV url.
 */
class KDAV_EXPORT DavCollectionsFetchJob : public DavJobBase
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
    void start() override;

    /**
     * Returns the list of fetched DAV collections.
     */
    Q_REQUIRED_RESULT DavCollection::List collections() const;

    /**
     * Return the DavUrl used by this job
     */
    Q_REQUIRED_RESULT DavUrl davUrl() const;

Q_SIGNALS:
    /**
     * This signal is emitted every time a new collection has been discovered.
     *
     * @param collectionUrl The URL of the discovered collection
     * @param configuredUrl The URL given to the job
     */
    void collectionDiscovered(int protocol, const QString &collectionUrl, const QString &configuredUrl);

private:
    void principalFetchFinished(KJob *);
    void collectionsFetchFinished(KJob *);
    void doCollectionsFetch(const QUrl &url);
    void subjobFinished();

    Q_DECLARE_PRIVATE(DavCollectionsFetchJob)
};
}

#endif
