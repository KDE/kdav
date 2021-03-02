/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMMODIFYJOB_H
#define KDAV_DAVITEMMODIFYJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavItemModifyJobPrivate;

/**
 * @class DavItemModifyJob davitemmodifyjob.h <KDAV/DavItemModifyJob>
 *
 * @short A job that modifies a DAV item on the DAV server.
 */
class KDAV_EXPORT DavItemModifyJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV item modify job.
     *
     * @param item The item that shall be modified.
     * @param parent The parent object.
     */
    explicit DavItemModifyJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the modified item including the updated ETag information.
     */
    Q_REQUIRED_RESULT DavItem item() const;

    Q_REQUIRED_RESULT QUrl itemUrl() const;

    /**
     * Returns the item that triggered the conflict, if any.
     */
    Q_REQUIRED_RESULT DavItem freshItem() const;

    /**
     * Returns the response code we got when fetching the fresh item.
     */
    Q_REQUIRED_RESULT int freshResponseCode() const;

private:
    Q_DECLARE_PRIVATE(DavItemModifyJob)
};
}

#endif
