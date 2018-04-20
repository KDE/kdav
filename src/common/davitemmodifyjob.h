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

#ifndef KDAV_DAVITEMMODIFYJOB_H
#define KDAV_DAVITEMMODIFYJOB_H

#include "kpimkdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV {
/**
 * @short A job that modifies a DAV item on the DAV server.
 */
class KPIMKDAV_EXPORT DavItemModifyJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav item modify job.
     *
     * @param item The item that shall be modified.
     * @param parent The parent object.
     */
    DavItemModifyJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the modified item including the updated etag information.
     */
    DavItem item() const;

    QUrl itemUrl() const;

    /**
     * Returns the item that triggered the conflict, if any.
     */
    DavItem freshItem() const;

    /**
     * Returns the response code we got when fetching the fresh item.
     */
    int freshResponseCode() const;

private:
    void davJobFinished(KJob *);
    void itemRefreshed(KJob *);
    void conflictingItemFetched(KJob *);
    DavItem mItem;
    DavItem mFreshItem;
    int mFreshResponseCode;
};
}

#endif
