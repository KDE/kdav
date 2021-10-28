/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTIONSMULTIFETCHJOB_H
#define KDAV_DAVCOLLECTIONSMULTIFETCHJOB_H

#include "kdav_export.h"

#include "davcollection.h"
#include "davurl.h"

#include <KCompositeJob>

#include <memory>

namespace KDAV
{
class DavCollectionsMultiFetchJobPrivate;

/**
 * @class DavCollectionsMultiFetchJob davcollectionsmultifetchjob.h <KDAV/DavCollectionsMultiFetchJob>
 *
 * @short A job that fetches all DAV collection.
 *
 * This job is used to fetch all DAV collection that are available
 * under a certain list of DAV URLs.
 *
 * @note This class just combines multiple calls of DavCollectionsFetchJob
 *       into one job.
 */
class KDAV_EXPORT DavCollectionsMultiFetchJob : public KCompositeJob
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV collections multi fetch job.
     *
     * @param urls The list of DAV URLs whose sub collections shall be fetched.
     * @param parent The parent object.
     */
    explicit DavCollectionsMultiFetchJob(const DavUrl::List &urls, QObject *parent = nullptr);
    ~DavCollectionsMultiFetchJob() override;

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the list of fetched DAV collections.
     */
    Q_REQUIRED_RESULT DavCollection::List collections() const;

Q_SIGNALS:
    /**
     * This signal is emitted every time a new collection has been discovered.
     *
     * @param collectionUrl The URL of the discovered collection
     * @param configuredUrl The URL given to the job
     */
    void collectionDiscovered(KDAV::Protocol protocol, const QString &collectionUrl, const QString &configuredUrl);

private:
    void slotResult(KJob *) override;

    const std::unique_ptr<DavCollectionsMultiFetchJobPrivate> d;
};
}

#endif
