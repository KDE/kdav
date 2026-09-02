/*
 *  SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KDAV_DAVITEMSSYNCJOB_H
#define KDAV_DAVITEMSSYNCJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"

namespace KDAV
{
class DavUrl;
class DavItemsSyncJobPrivate;

/*!
 * \class KDAV::DavItemsSyncJob
 * \inheaderfile KDAV/DavItemsSyncJob
 * \inmodule KDAV
 *
 * \brief A job that uses the sync-token to fetch only new changes of a DAV collection.
 * \since 6.30
 */
class KDAV_EXPORT DavItemsSyncJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DAV items sync job.
     *
     * \a url The URL of the DAV collection.
     *
     * \a parent The parent object.
     */
    DavItemsSyncJob(const DavUrl &url, const QString &syncToken, QObject *parent = nullptr);

    ~DavItemsSyncJob() override;

    /*!
     * Starts the job.
     */
    void start() override;

    /*!
     * Returns the list of items that were changed on the server.
     * \note This sync-collection job cannot fetch mimeTypes, it will be empty
     */
    [[nodiscard]] DavItem::List changedItems() const;

    /*!
     * Returns the list of items URLs that were deleted on the server.
     */
    [[nodiscard]] QStringList deletedItems() const;

    /*!
     * Returns the new sync token, empty if none returned by the server.
     */
    [[nodiscard]] QString newSyncToken() const;

private:
    Q_DECLARE_PRIVATE(DavItemsSyncJob)
};
}

#endif
