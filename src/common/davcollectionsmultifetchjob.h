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

/*!
 * \class KDAV::DavCollectionsMultiFetchJob
 * \inheaderfile KDAV/DavCollectionsMultiFetchJob
 * \inmodule KDAV
 *
 * \brief A job that fetches all DAV collection.
 *
 * This job is used to fetch all DAV collection that are available
 * under a certain list of DAV URLs.
 *
 * \note This class just combines multiple calls of DavCollectionsFetchJob
 *       into one job.
 */
class KDAV_EXPORT DavCollectionsMultiFetchJob : public KCompositeJob
{
    Q_OBJECT

public:
    /*!
     * Creates a new DAV collections multi fetch job.
     *
     * \a urls The list of DAV URLs whose sub collections shall be fetched.
     *
     * \a parent The parent object.
     */
    explicit DavCollectionsMultiFetchJob(const DavUrl::List &urls, QObject *parent = nullptr);
    ~DavCollectionsMultiFetchJob() override;

    /*!
     * Starts the job.
     */
    void start() override;

    /*!
     * Returns the list of fetched DAV collections.
     */
    Q_REQUIRED_RESULT DavCollection::List collections() const;

Q_SIGNALS:
    /*!
     * This signal is emitted every time a new collection has been discovered.
     *
     * \a collectionUrl The URL of the discovered collection
     *
     * \a configuredUrl The URL given to the job
     */
    void collectionDiscovered(KDAV::Protocol protocol, const QString &collectionUrl, const QString &configuredUrl);

private:
    void slotResult(KJob *) override;

    const std::unique_ptr<DavCollectionsMultiFetchJobPrivate> d;
};
}

#endif
