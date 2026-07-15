/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPUSHDONTNOTIFY_H
#define KDAV_DAVPUSHDONTNOTIFY_H

#include "kdav_export.h"

#include <QSharedDataPointer>

class DavPushDontNotifyPrivate;

namespace KDAV
{
/*!
 * \class KDAV::DavPushDontNotify
 * \inheaderfile KDAV/DavPushDontNotify
 * \inmodule KDAV
 *
 * \brief A helper class to store DavPush DontNotify data.
 * \sa https://bitfireat.github.io/webdav-push/draft-bitfire-webdav-push-00.html#name-suppressing-notifications
 *
 * \since 6.29
 */
class KDAV_EXPORT DavPushDontNotify
{
public:
    /*!
     * Creates an empty DavPushDontNotify.
     */
    DavPushDontNotify();

    DavPushDontNotify(const DavPushDontNotify &other);
    DavPushDontNotify(DavPushDontNotify &&) noexcept;
    DavPushDontNotify &operator=(const DavPushDontNotify &other);
    DavPushDontNotify &operator=(DavPushDontNotify &&) noexcept;
    ~DavPushDontNotify();

    /*!
     * Returns a DavPushDontNotify that ignores all notifications.
     */
    [[nodiscard]] static DavPushDontNotify ignoreAll();

    /*!
     * Returns a DavPushDontNotify that ignores the given urls.
     */
    [[nodiscard]] static DavPushDontNotify ignoreUrls(const QStringList &urls);

    /*!
     * Returns whether this DavPushDontNotify contains any data.
     */
    [[nodiscard]] bool isNull() const;

    /*!
     * Ignores the given URLs.
     * Note: this function disables the ignore-all
     */
    void setIgnoredUrls(const QStringList &ignoredUrls);

    /*!
     * Returns the list of ignored URLs.
     */
    [[nodiscard]] QStringList ignoredUrls() const;

    /*!
     * Ignore all notifications (uses the '*' pattern)
     * Note: this function clears the list of ignored URLs
     */
    void setIgnoreAll(bool ignoreAll);

    /*!
     * Returns whether all notifications are ignored.
     */
    [[nodiscard]] bool isIgnoreAll() const;

    /*!
     * Returns the header name for DavPushDontNotify
     */
    [[nodiscard]] static QString davHeaderName();

    /*!
     * Returns the header value for this DavPush DontNotify.
     */
    [[nodiscard]] QString davHeaderValue() const;

    /*!
     * Returns the header for this DavPush DontNotify.
     */
    [[nodiscard]] QString davHeader() const;

private:
    QSharedDataPointer<DavPushDontNotifyPrivate> d;
};

}

Q_DECLARE_TYPEINFO(KDAV::DavPushDontNotify, Q_RELOCATABLE_TYPE);

#endif
