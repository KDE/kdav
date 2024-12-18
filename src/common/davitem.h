/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVITEM_H
#define KDAV_DAVITEM_H

#include "kdav_export.h"

#include <QByteArray>
#include <QDataStream>
#include <QList>
#include <QSharedDataPointer>
#include <QString>

class DavItemPrivate;

namespace KDAV
{
class DavUrl;
}

namespace KDAV
{
/*!
 * \class KDAV::DavItem
 * \inheaderfile KDAV/DavItem
 * \inmodule KDAV
 *
 * \brief A helper class to store information about DAV resources.
 *
 * This class is used as container to transfer information about DAV
 * resources between the Akonadi resource and the DAV jobs.
 *
 * \note While the DAV RFC names them DAV resource we call them items
 *       to comply to Akonadi terminology.
 */
class KDAV_EXPORT DavItem
{
public:
    /*!
     * Defines a list of DAV item objects.
     */
    typedef QList<DavItem> List;

    /*!
     * Creates an empty DAV item.
     */
    DavItem();

    /*!
     * Creates a new DAV item.
     *
     * \a url The URL that identifies the item.
     *
     * \a contentType The content type of the item.
     *
     * \a data The actual raw content data of the item.
     *
     * \a etag The ETag of the item.
     */
    DavItem(const DavUrl &url, const QString &contentType, const QByteArray &data, const QString &etag);

    DavItem(const DavItem &other);
    DavItem(DavItem &&);
    DavItem &operator=(const DavItem &other);
    DavItem &operator=(DavItem &&);

    ~DavItem();

    /*!
     * Sets the \a url that identifies the item.
     */
    void setUrl(const DavUrl &url);

    /*!
     * Returns the URL that identifies the item.
     */
    Q_REQUIRED_RESULT DavUrl url() const;

    /*!
     * Sets the content \a type of the item.
     */
    void setContentType(const QString &type);

    /*!
     * Returns the content type of the item.
     */
    Q_REQUIRED_RESULT QString contentType() const;

    /*!
     * Sets the raw content \a data of the item.
     */
    void setData(const QByteArray &data);

    /*!
     * Returns the raw content data of the item.
     */
    Q_REQUIRED_RESULT QByteArray data() const;

    /*!
     * Sets the \a etag of the item.
     * \sa https://tools.ietf.org/html/rfc4918#section-8.6
     */
    void setEtag(const QString &etag);

    /*!
     * Returns the ETag of the item.
     * \sa https://tools.ietf.org/html/rfc4918#section-8.6
     */
    Q_REQUIRED_RESULT QString etag() const;

private:
    QSharedDataPointer<DavItemPrivate> d;
};

KDAV_EXPORT QDataStream &operator<<(QDataStream &out, const DavItem &item);
KDAV_EXPORT QDataStream &operator>>(QDataStream &in, DavItem &item);
}

Q_DECLARE_TYPEINFO(KDAV::DavItem, Q_RELOCATABLE_TYPE);

#endif
