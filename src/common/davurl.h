/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KDAV_DAVURL_H
#define KDAV_DAVURL_H

#include "kpimkdav_export.h"

#include "enums.h"

#include <QSharedDataPointer>
#include <QUrl>
#include <QVector>

namespace KDAV {
class DavUrlPrivate;
/**
 * @short A helper class to combine url and protocol of a DAV url.
 */
class KPIMKDAV_EXPORT DavUrl
{
public:
    /**
     * Defines a list of DAV url objects.
     */
    typedef QVector<DavUrl> List;

    /**
     * Creates an empty DAV url.
     */
    DavUrl();
    DavUrl(const DavUrl&);
    ~DavUrl();
    DavUrl& operator=(const DavUrl&);

    /**
     * Creates a new DAV url.
     *
     * @param url The url that identifies the DAV object.
     * @param protocol The DAV protocol dialect that is used to retrieve the DAV object.
     */
    DavUrl(const QUrl &url, Protocol protocol);

    /**
     * Sets the @p url that identifies the DAV object.
     */
    void setUrl(const QUrl &url);

    /**
     * Returns the url that identifies the DAV object.
     */
    Q_REQUIRED_RESULT QUrl url() const;

    /**
     * Returns the url in a user-friendly way without login information.
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

KPIMKDAV_EXPORT QDataStream &operator<<(QDataStream &out, const DavUrl &url);
KPIMKDAV_EXPORT QDataStream &operator>>(QDataStream &in, DavUrl &url);
}

Q_DECLARE_TYPEINFO(KDAV::DavUrl, Q_MOVABLE_TYPE);

#endif
