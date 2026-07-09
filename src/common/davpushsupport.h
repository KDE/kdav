/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPUSH_H
#define KDAV_DAVPUSH_H

#include "kdav_export.h"

#include <QSharedDataPointer>

class DavPushSupportPrivate;

namespace KDAV
{
/*!
 * \class KDAV::DavPushSupport
 * \inheaderfile KDAV/DavPushSupport
 * \inmodule KDAV
 *
 * \brief A helper class to store information about Support of DavPush.
 *
 * This class is used as a container to transfer DavPush support of
 * DavCollections from the dav server.
 * \sa https://bitfireat.github.io/webdav-push/draft-bitfire-webdav-push-00.html
 *
 * \since 6.29
 */
class KDAV_EXPORT DavPushSupport
{
public:
    /*!
     * Creates an empty DAV Push.
     */
    DavPushSupport();

    DavPushSupport(const DavPushSupport &other);
    DavPushSupport(DavPushSupport &&);
    DavPushSupport &operator=(const DavPushSupport &other);
    DavPushSupport &operator=(DavPushSupport &&);
    ~DavPushSupport();

    bool operator==(const DavPushSupport &) const;

    /*!
     * Returns true if this DavPushSupport is valid
     */
    [[nodiscard]] bool isValid() const;

    /*!
     * Sets this push's topic
     */
    void setTopic(const QString &topic);

    /*!
     * Returns this push's topic, empty if no value was found
     */
    [[nodiscard]] QString topic() const;

    /*!
     * Sets this push's vapid public key
     * vapid must be in base64url (not base64)
     */
    void setVapidPublicKey(const QByteArray &vapidPublicKey);

    /*!
     * Returns this push's vapid public key, empty if no value was found
     * vapid format is returned in base64url (not base64)
     */
    [[nodiscard]] QByteArray vapidPublicKey() const;

private:
    QSharedDataPointer<DavPushSupportPrivate> d;
};

}

Q_DECLARE_TYPEINFO(KDAV::DavPushSupport, Q_RELOCATABLE_TYPE);

#endif
