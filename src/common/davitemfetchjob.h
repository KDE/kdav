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

#ifndef KDAV_DAVITEMFETCHJOB_H
#define KDAV_DAVITEMFETCHJOB_H

#include "kpimkdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV {
class DavItemFetchJobPrivate;

/**
 * @short A job that fetches a DAV item from the DAV server.
 */
class KPIMKDAV_EXPORT DavItemFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav item fetch job.
     *
     * @param item The item that shall be fetched.
     * @param parent The parent object.
     */
    explicit DavItemFetchJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the fetched item including current etag information.
     */
    Q_REQUIRED_RESULT DavItem item() const;

private:
    void davJobFinished(KJob *);
    Q_DECLARE_PRIVATE(DavItemFetchJob)
};
}

#endif
