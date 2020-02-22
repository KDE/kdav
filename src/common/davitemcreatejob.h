/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEMCREATEJOB_H
#define KDAV_DAVITEMCREATEJOB_H

#include "kdav_export.h"

#include "davitem.h"
#include "davjobbase.h"
#include "davurl.h"

namespace KDAV {
class DavItemCreateJobPrivate;

/**
 * @short A job to create a DAV item on the DAV server.
 */
class KDAV_EXPORT DavItemCreateJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav item create job.
     *
     * @param item The item that shall be created.
     * @param parent The parent object.
     */
    explicit DavItemCreateJob(const DavItem &item, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the created DAV item including the correct identifier url
     * and current etag information.
     */
    Q_REQUIRED_RESULT DavItem item() const;

    Q_REQUIRED_RESULT QUrl itemUrl() const;

private:
    void davJobFinished(KJob *);
    void itemRefreshed(KJob *);

    Q_DECLARE_PRIVATE(DavItemCreateJob)
};
}

#endif
