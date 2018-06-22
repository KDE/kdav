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

#ifndef KDAV_DAVITEMSLISTJOB_H
#define KDAV_DAVITEMSLISTJOB_H

#include "kpimkdav_export.h"

#include "davitem.h"
#include "davjobbase.h"

#include <memory>

#include <QStringList>

class DavItemsListJobPrivate;

namespace KDAV {
class EtagCache;
class DavUrl;

/**
 * @short A job that lists all DAV items inside a DAV collection.
 */
class KPIMKDAV_EXPORT DavItemsListJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav items list job.
     *
     * @param url The url of the DAV collection.
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
     * Returns the list of items seen including identifier url and etag information.
     */
    Q_REQUIRED_RESULT DavItem::List items() const;

    /**
     * Returns the list of items that were changed on the server.
     */
    Q_REQUIRED_RESULT DavItem::List changedItems() const;

    /**
     * Returns the list of items URLs that were not seen in the backend.
     * As this is based on the etag cache this may contain dependent items.
     */
    Q_REQUIRED_RESULT QStringList deletedItems() const;

private:
    void davJobFinished(KJob *);
    std::unique_ptr<DavItemsListJobPrivate> d;
};
}

#endif
