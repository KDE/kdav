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
 * \since 6.29.0
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
     * Creates a new DavPush registration
     *
     * \a url The DAV URL of the collection to register a DavPush subscription to.
     * \a pushEndpoint The URL of the subscription.
     * \a subscriptionPublicKey The public encryption key of the subscription in base64url (not base64)
     * \a authToken The authentication token of the subscription.
     * \a expiration The expiration date of the subscription.
     */
    explicit DavPushRegistration(const QUrl &pushEndpoint, const QByteArray &subscriptionPublicKey, const QByteArray &authToken, const QDateTime &expiration);

    void setPushEndpoint(const QUrl &pushEndpoint);
    [[nodiscard]] QUrl pushEndpoint() const;

    void setSubscriptionPublicKey(const QByteArray &subscriptionPublicKey);
    [[nodiscard]] QByteArray subscriptionPublicKey() const;

    void setAuthToken(const QByteArray &authToken);
    [[nodiscard]] QByteArray authToken() const;

    void setExpiration(const QDateTime &expiration);
    [[nodiscard]] QDateTime expiration() const;

private:
    QSharedDataPointer<DavPushRegistrationPrivate> d;
};
}
