/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVURL_H
#define KDAV_DAVURL_H

#include "kdav_export.h"

#include "enums.h"

#include <QSharedDataPointer>
#include <QUrl>
#include <QVector>

namespace KDAV
{
class DavUrlPrivate;
/**
 * @class DavUrl davurl.h <KDAV/DavUrl>
 *
 * @short A helper class to combine URL and protocol of a DAV URL.
 */
class KDAV_EXPORT DavUrl
{
public:
    /**
     * Defines a list of DAV URL objects.
     */
    typedef QVector<DavUrl> List;

    /**
     * Creates an empty DAV URL.
     */
    DavUrl();
    DavUrl(const DavUrl &);
    DavUrl(DavUrl &&);
    ~DavUrl();
    DavUrl &operator=(const DavUrl &);
    DavUrl &operator=(DavUrl &&);

    /**
     * Creates a new DAV URL.
     *
     * @param url The URL that identifies the DAV object.
     * @param protocol The DAV protocol dialect that is used to retrieve the DAV object.
     */
    DavUrl(const QUrl &url, Protocol protocol);

    /**
     * Sets the @p url that identifies the DAV object.
     */
    void setUrl(const QUrl &url);

    /**
     * Returns the URL that identifies the DAV object.
     */
    Q_REQUIRED_RESULT QUrl url() const;

    /**
     * Returns the URL in a user-friendly way without login information.
     */
    Q_REQUIRED_RESULT QString toDisplayString() const;

    /**
     * Sets the DAV @p protocol dialect that is used to retrieve the DAV object.
     */
    void setProtocol(Protocol protocol);

    /**
     * Returns the DAV protocol dialect that is used to retrieve the DAV object.
     */
    Q_REQUIRED_RESULT Protocol protocol() const;

private:
    QSharedDataPointer<DavUrlPrivate> d;
};

KDAV_EXPORT QDataStream &operator<<(QDataStream &out, const DavUrl &url);
KDAV_EXPORT QDataStream &operator>>(QDataStream &in, DavUrl &url);
}

Q_DECLARE_TYPEINFO(KDAV::DavUrl, Q_MOVABLE_TYPE);

#endif
