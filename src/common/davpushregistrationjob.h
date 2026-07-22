// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kdav_export.h"

#include "davjobbase.h"

namespace KDAV
{
class DavUrl;
class DavPushRegistration;
}

namespace KDAV
{
class DavPushRegistrationJobPrivate;

/*!
 * \class KDAV::DavPushRegistrationJob
 * \inheaderfile KDAV/DavPushRegistrationJob
 * \inmodule KDAV
 *
 * \brief A job to register a DavPush subscription for a DAV collection.
 * \since 6.29.0
 */
class KDAV_EXPORT DavPushRegistrationJob : public DavJobBase
{
    Q_OBJECT

public:
    /*!
     * Creates a new DavPush registration job.
     *
     * \a url The DAV URL of the collection to register a DavPush subscription to.
     * \a davPushRegistration The data for the subscription.
     * \a parent The parent object.
     */
    explicit DavPushRegistrationJob(const DavUrl &url, const DavPushRegistration &davPushRegistration, QObject *parent = nullptr);

    /*!
     * Starts the job.
     */
    void start() override;

    /*!
     * Returns the server's response absolute registration Url.
     */
    [[nodiscard]] QUrl registrationUrl() const;

    /*!
     * Returns the server's response registration expiration.
     */
    [[nodiscard]] QDateTime expirationDate() const;

private:
    Q_DECLARE_PRIVATE(DavPushRegistrationJob)
};
}
