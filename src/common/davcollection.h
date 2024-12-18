/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTION_H
#define KDAV_DAVCOLLECTION_H

#include "kdav_export.h"

#include "enums.h"

#include <QList>
#include <QSharedDataPointer>
#include <QString>

class QColor;

class DavCollectionPrivate;

namespace KDAV
{
class DavUrl;
}

namespace KDAV
{
/*!
 * \class KDAV::DavCollection
 * \inheaderfile KDAV/DavCollection
 * \inmodule KDAV
 *
 * \brief A helper class to store information about DAV collection.
 *
 * This class is used as container to transfer information about DAV
 * collections between the Akonadi resource and the DAV jobs.
 */
class KDAV_EXPORT DavCollection
{
public:
    /*!
     * Defines a list of DAV collection objects.
     */
    typedef QList<DavCollection> List;

    /*!
     * Describes the possible content type of the DAV collection.
     *
     * \value Events The collection can contain event DAV resources.
     * \value Todos The collection can contain todo DAV resources.
     * \value Contacts The collection can contain contact DAV resources.
     * \value FreeBusy The collection can contain free/busy information.
     * \value Journal The collection can contain journal DAV resources.
     * \value Calendar The collection can contain anything calendar-related.
     */
    enum ContentType {
        Events = 1,
        Todos = 2,
        Contacts = 4,
        FreeBusy = 8,
        Journal = 16,
        Calendar = 32,
    };
    Q_DECLARE_FLAGS(ContentTypes, ContentType)

    /*!
     * Creates an empty DAV collection.
     */
    DavCollection();

    /*!
     * Creates a new DAV collection.
     *
     * \a url The URL that identifies the collection.
     *
     * \a displayName The display name of the collection.
     *
     * \a contentTypes The possible content types of the collection.
     */
    DavCollection(const DavUrl &url, const QString &displayName, ContentTypes contentTypes);

    DavCollection(const DavCollection &other);
    DavCollection(DavCollection &&);
    DavCollection &operator=(const DavCollection &other);
    DavCollection &operator=(DavCollection &&);

    ~DavCollection();

    /*!
     * Sets this collection CTag.
     * \sa https://github.com/apple/ccs-calendarserver/blob/master/doc/Extensions/caldav-ctag.txt
     */
    void setCTag(const QString &ctag);

    /*!
     * Returns this collection CTag. The returned value will be empty
     * if no CTag was found.
     * \sa https://github.com/apple/ccs-calendarserver/blob/master/doc/Extensions/caldav-ctag.txt
     */
    Q_REQUIRED_RESULT QString CTag() const;

    /*!
     * Sets the \a url that identifies the collection.
     */
    void setUrl(const DavUrl &url);

    /*!
     * Returns the URL that identifies the collection.
     */
    Q_REQUIRED_RESULT DavUrl url() const;

    /*!
     * Sets the display \a name of the collection.
     */
    void setDisplayName(const QString &name);

    /*!
     * Returns the display name of the collection.
     */
    Q_REQUIRED_RESULT QString displayName() const;

    /*!
     * Sets the color for this collection
     */
    void setColor(const QColor &color);

    /*!
     * Return the color of the collection, or an empty string if
     * none was provided by the backend.
     */
    Q_REQUIRED_RESULT QColor color() const;

    /*!
     * Sets the possible content \a types of the collection.
     */
    void setContentTypes(ContentTypes types);

    /*!
     * Returns the possible content types of the collection.
     */
    Q_REQUIRED_RESULT ContentTypes contentTypes() const;

    /*!
     * Sets the privileges on this collection.
     */
    void setPrivileges(Privileges privs);

    /*!
     * Returns the privileges on this collection.
     */
    Q_REQUIRED_RESULT Privileges privileges() const;

private:
    QSharedDataPointer<DavCollectionPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DavCollection::ContentTypes)

}

Q_DECLARE_TYPEINFO(KDAV::DavCollection, Q_RELOCATABLE_TYPE);

#endif
