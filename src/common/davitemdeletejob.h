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

#ifndef KDAV_DAVITEMDELETEJOB_H
#define KDAV_DAVITEMDELETEJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV {
class DavItemDeleteJobPrivate;

/**
 * @short A job to delete a DAV item on the DAV server.
 */
class KDAV_EXPORT DavItemDeleteJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav item delete job.
     *
     * @param item The item that shall be deleted.
     * @param parent The parent object.
     */
    explicit DavItemDeleteJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the item that triggered the conflict, if any.
     */
    Q_REQUIRED_RESULT DavItem freshItem() const;

    /**
     * Returns the response code we got when fetching the fresh item.
     */
    Q_REQUIRED_RESULT int freshResponseCode() const;

private:
    void davJobFinished(KJob *);
    void conflictingItemFetched(KJob *);

    Q_DECLARE_PRIVATE(DavItemDeleteJob)
};
}

#endif
