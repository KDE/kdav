// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kdav_export.h"

#include "davjobbase.h"

namespace KDAV
{
class DavUrl;
}

namespace KDAV
{
class DavPushUnregistrationJobPrivate;

/*!
 * \class KDAV::DavPushUnregistrationJob
 * \inheaderfile KDAV/DavPushUnregistrationJob
 * \inmodule KDAV
 *
 * \brief A job to unregister a DavPush subscription from a DAV collection.
 * \since 6.31
 */
class KDAV_EXPORT DavPushUnregistrationJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DavPush unregistration job.
     *
     * \a url The DAV URL of the collection to unregister a DavPush subscription from
     * \a url The registration url that has been returned by the registration
     * \a parent The parent object
     */
    explicit DavPushUnregistrationJob(const DavUrl &url, const QUrl &registrationUrl, QObject *parent = nullptr);

    /*!
     * Starts the job.
     */
    void start() override;

private:
    Q_DECLARE_PRIVATE(DavPushUnregistrationJob)
};
}
