/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMSLISTJOB_H
#define KDAV_DAVITEMSLISTJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"

#include <memory>

#include <QStringList>

namespace KDAV
{
class EtagCache;
class DavUrl;
class DavItemsListJobPrivate;

/**
 * @class DavItemsListJob davitemslistjob.h <KDAV/DavItemsListJob>
 *
 * @short A job that lists all DAV items inside a DAV collection.
 */
class KDAV_EXPORT DavItemsListJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV items list job.
     *
     * @param url The URL of the DAV collection.
     * @param parent The parent object.
     */
    DavItemsListJob(const DavUrl &url, const std::shared_ptr<EtagCache> &cache, QObject *parent = nullptr);

    ~DavItemsListJob() override;

    /**
     * Limits the mime types of the items requested.
     *
     * If no mime type is given then all will be requested.
     *
     * @param types The list of mime types to include
     */
    void setContentMimeTypes(const QStringList &types);

    /**
     * Sets the start and end time to list items for.
     *
     * @param start The range start, in format "date with UTC time"
     * @param end The range end, in format "date with UTC time"
     */
    void setTimeRange(const QString &start, const QString &end);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the list of items seen including identifier URL and ETag information.
     */
    Q_REQUIRED_RESULT DavItem::List items() const;

    /**
     * Returns the list of items that were changed on the server.
     */
    Q_REQUIRED_RESULT DavItem::List changedItems() const;

    /**
     * Returns the list of items URLs that were not seen in the backend.
     * As this is based on the ETag cache this may contain dependent items.
     */
    Q_REQUIRED_RESULT QStringList deletedItems() const;

private:
    Q_DECLARE_PRIVATE(DavItemsListJob)
};
}

#endif
