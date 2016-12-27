/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef KDAV_DAVURL_H
#define KDAV_DAVURL_H

#include "libkdav_export.h"

#include "enums.h"

#include <QtCore/QUrl>
#include <QtCore/QVector>

namespace KDAV
{

/**
 * @short A helper class to combine url and protocol of a DAV url.
 */
class LIBKDAV_EXPORT DavUrl
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
    QUrl url() const;

    /**
     * Returns the url in a userfriendly way without login informations.
     */
    QString toDisplayString() const;

    /**
     * Sets the DAV @p protocol dialect that is used to retrieve the DAV object.
     */
    void setProtocol(Protocol protocol);

    /**
     * Returns the DAV protocol dialect that is used to retrieve the DAV object.
     */
    Protocol protocol() const;

private:
    QUrl mUrl;
    Protocol mProtocol;
};

QDataStream &operator<<(QDataStream &out, const DavUrl &url);
QDataStream &operator>>(QDataStream &in, DavUrl &url);

}

Q_DECLARE_TYPEINFO(KDAV::DavUrl, Q_MOVABLE_TYPE);

#endif
