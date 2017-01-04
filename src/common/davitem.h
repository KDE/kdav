/*
    Copyright (c) 2009 Grégory Oestreicher <greg@kamago.net>

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

#ifndef KDAV_DAVITEM_H
#define KDAV_DAVITEM_H

#include "kpimkdav_export.h"

#include <memory>

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QString>
#include <QtCore/QVector>


class DavItemPrivate;

namespace KDAV
{
    class DavUrl;
}

namespace KDAV
{

/**
 * @short A helper class to store information about DAV resources.
 *
 * This class is used as container to transfer information about DAV
 * resources between the Akonadi resource and the DAV jobs.
 *
 * @note While the DAV RFC names them DAV resource we call them items
 *       to comply to Akonadi terminology.
 */
class KPIMKDAV_EXPORT DavItem
{
public:
    /**
     * Defines a list of DAV item objects.
     */
    typedef QVector<DavItem> List;

    /**
     * Creates an empty DAV item.
     */
    DavItem();

    /**
     * Creates a new DAV item.
     *
     * @param url The url that identifies the item.
     * @param contentType The content type of the item.
     * @param data The actual raw content data of the item.
     * @param etag The etag of the item.
     */
    DavItem(const DavUrl &url, const QString &contentType, const QByteArray &data, const QString &etag);

    DavItem(const DavItem &other);
    DavItem &operator=(const DavItem &other);

    ~DavItem();

    /**
     * Sets the @p url that identifies the item.
     */
    void setUrl(const DavUrl &url);

    /**
     * Returns the url that identifies the item.
     */
    DavUrl url() const;

    /**
     * Sets the content @p type of the item.
     */
    void setContentType(const QString &type);

    /**
     * Returns the content type of the item.
     */
    QString contentType() const;

    /**
     * Sets the raw content @p data of the item.
     */
    void setData(const QByteArray &data);

    /**
     * Returns the raw content data of the item.
     */
    QByteArray data() const;

    /**
     * Sets the @p etag of the item.
     */
    void setEtag(const QString &etag);

    /**
     * Returns the etag of the item.
     */
    QString etag() const;

private:
    std::unique_ptr<DavItemPrivate> d;
};

KPIMKDAV_EXPORT QDataStream &operator<<(QDataStream &out, const DavItem &item);
KPIMKDAV_EXPORT QDataStream &operator>>(QDataStream &in, DavItem &item);

}

Q_DECLARE_TYPEINFO(KDAV::DavItem, Q_MOVABLE_TYPE);

#endif
