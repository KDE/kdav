// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "kdav_export.h"

#include <QSharedDataPointer>

class QDateTime;
class QByteArray;
class QUrl;

namespace KDAV
{
class DavPushRegistrationPrivate;

/*!
 * \class KDAV::DavPushRegistration
 * \inheaderfile KDAV/DavPushRegistration
 * \inmodule KDAV
 *
 * \brief A class containing data for a DavPush registration.
 * \since 6.31
 */
class KDAV_EXPORT DavPushRegistration
{
public:
    /*!
     * Creates an empty DavPush registration
     */
    DavPushRegistration();
    DavPushRegistration(const DavPushRegistration &);
    DavPushRegistration &operator=(const DavPushRegistration &);
    DavPushRegistration(DavPushRegistration &&) noexcept;
    DavPushRegistration &operator=(DavPushRegistration &&) noexcept;
    ~DavPushRegistration();

    /*!
     * The URL of the subscription
     */
    [[nodiscard]] QUrl pushEndpoint() const;
    void setPushEndpoint(const QUrl &pushEndpoint);

    /*!
     * The public encryption key of the subscription in base64url (not base64)
     */
    [[nodiscard]] QByteArray subscriptionPublicKey() const;
    void setSubscriptionPublicKey(const QByteArray &subscriptionPublicKey);

    /*!
     * The authentication token of the subscription
     */
    [[nodiscard]] QByteArray authToken() const;
    void setAuthToken(const QByteArray &authToken);

    /*!
     * The expiration date of the subscription
     */
    [[nodiscard]] QDateTime expiration() const;
    void setExpiration(const QDateTime &expiration);

private:
    QSharedDataPointer<DavPushRegistrationPrivate> d;
};
}
