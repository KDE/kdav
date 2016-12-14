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

#ifndef DAVITEMSLISTJOB_H
#define DAVITEMSLISTJOB_H

#include "libkdav_export.h"

#include "davitem.h"
#include "davutils.h"

#include <KCoreAddons/KJob>

#include <QtCore/QSet>
#include <QtCore/QStringList>

class EtagCache;

/**
 * @short A job that lists all DAV items inside a DAV collection.
 */
class LIBKDAV_EXPORT DavItemsListJob : public KJob
{
    Q_OBJECT

public:
    /**
     * Creates a new dav items list job.
     *
     * @param url The url of the DAV collection.
     * @param parent The parent object.
     */
    DavItemsListJob(const DavUtils::DavUrl &url, const EtagCache *cache, QObject *parent = Q_NULLPTR);

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
    void start() Q_DECL_OVERRIDE;

    /**
     * Returns the list of items seen including identifier url and etag information.
     */
    DavItem::List items() const;

    /**
     * Returns the list of items that were changed on the server.
     */
    DavItem::List changedItems() const;

    /**
     * Returns the list of items URLs that were not seen in the backend.
     * As this is based on the etag cache this may contain dependent items.
     */
    QStringList deletedItems() const;

private Q_SLOTS:
    void davJobFinished(KJob *);

private:
    DavUtils::DavUrl mUrl;
    const EtagCache *mEtagCache;
    QStringList mMimeTypes;
    QString mRangeStart;
    QString mRangeEnd;
    DavItem::List mItems;
    QSet<QString> mSeenUrls; // to prevent events duplication with some servers
    DavItem::List mChangedItems;
    QStringList mDeletedItems;
    uint mSubJobCount;
};

#endif