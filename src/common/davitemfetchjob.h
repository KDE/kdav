/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMFETCHJOB_H
#define KDAV_DAVITEMFETCHJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV
{
class DavItemFetchJobPrivate;

/**
 * @class DavItemFetchJob davitemfetchjob.h <KDAV/DavItemFetchJob>
 *
 * @short A job that fetches a DAV item from the DAV server.
 */
class KDAV_EXPORT DavItemFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV item fetch job.
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
     * Returns the fetched item including current ETag information.
     */
    Q_REQUIRED_RESULT DavItem item() const;

private:
    Q_DECLARE_PRIVATE(DavItemFetchJob)
};
}

#endif
