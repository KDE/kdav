/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMMOVEJOB_H
#define KDAV_DAVITEMMOVEJOB_H

#include "kdav_export.h"

#include "davjobbase.h"

namespace KDAV
{
class DavItem;
}

namespace KDAV
{
class DavItemMoveJobPrivate;

/*!
 * \class KDAV::DavItemMoveJob
 * \inheaderfile KDAV/DavItemMoveJob
 * \inmodule KDAV
 *
 * \brief A job to move a DAV item from one location to another.
 *
 * \since 6.30
 */
class KDAV_EXPORT DavItemMoveJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DAV item move job.
     *
     * \a item The item that shall be moved.
     * \a destination The url the item will be moved to.
     * \a parent The parent object.
     */
    explicit DavItemMoveJob(const DavItem &item, const QUrl &destination, QObject *parent = nullptr);

    /*!
     * Starts the job.
     */
    void start() override;

private:
    Q_DECLARE_PRIVATE(DavItemMoveJob)
};
}

#endif
