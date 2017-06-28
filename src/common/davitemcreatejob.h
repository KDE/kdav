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

#ifndef KDAV2_DAVITEMCREATEJOB_H
#define KDAV2_DAVITEMCREATEJOB_H

#include "kpimkdav2_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV2
{

/**
 * @short A job to create a DAV item on the DAV server.
 */
class KPIMKDAV2_EXPORT DavItemCreateJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav item create job.
     *
     * @param item The item that shall be created.
     * @param parent The parent object.
     */
    DavItemCreateJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() Q_DECL_OVERRIDE;

    /**
     * Returns the created DAV item including the correct identifier url
     * and current etag information.
     */
    DavItem item() const;

    QUrl itemUrl() const;

private Q_SLOTS:
    void davJobFinished(KJob *);
    void itemRefreshed(KJob *);

private:
    DavItem mItem;
    int mRedirectCount;
};

}

#endif
