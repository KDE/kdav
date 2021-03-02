/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMDELETEJOB_H
#define KDAV_DAVITEMDELETEJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavItemDeleteJobPrivate;

/**
 * @class DavItemDeleteJob davitemdeletejob.h <KDAV/DavItemDeleteJob>
 *
 * @short A job to delete a DAV item on the DAV server.
 */
class KDAV_EXPORT DavItemDeleteJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV item delete job.
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
    Q_DECLARE_PRIVATE(DavItemDeleteJob)
};
}

#endif
