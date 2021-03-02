/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVCOLLECTION_H
#define KDAV_DAVCOLLECTION_H

#include "kdav_export.h"

#include "enums.h"

#include <QSharedDataPointer>
#include <QString>
#include <QVector>

class QColor;

class DavCollectionPrivate;

namespace KDAV
{
class DavUrl;
}

namespace KDAV
{
/**
 * @class DavCollection davcollection.h <KDAV/DavCollection>
 *
 * @short A helper class to store information about DAV collection.
 *
 * This class is used as container to transfer information about DAV
 * collections between the Akonadi resource and the DAV jobs.
 */
class KDAV_EXPORT DavCollection
{
public:
    /**
     * Defines a list of DAV collection objects.
     */
    typedef QVector<DavCollection> List;

    /**
     * Describes the possible content type of the DAV collection.
     */
    enum ContentType {
        Events = 1, ///< The collection can contain event DAV resources.
        Todos = 2, ///< The collection can contain todo DAV resources.
        Contacts = 4, ///< The collection can contain contact DAV resources.
        FreeBusy = 8, ///< The collection can contain free/busy information.
        Journal = 16, ///< The collection can contain journal DAV resources.
        Calendar = 32, ///< The collection can contain anything calendar-related.
    };
    Q_DECLARE_FLAGS(ContentTypes, ContentType)

    /**
     * Creates an empty DAV collection.
     */
    DavCollection();

    /**
     * Creates a new DAV collection.
     *
     * @param url The URL that identifies the collection.
     * @param displayName The display name of the collection.
     * @param contentTypes The possible content types of the collection.
     */
    DavCollection(const DavUrl &url, const QString &displayName, ContentTypes contentTypes);

    DavCollection(const DavCollection &other);
    DavCollection(DavCollection &&);
    DavCollection &operator=(const DavCollection &other);
    DavCollection &operator=(DavCollection &&);

    ~DavCollection();

    /**
     * Sets this collection CTag.
     * @see https://github.com/apple/ccs-calendarserver/blob/master/doc/Extensions/caldav-ctag.txt
     */
    void setCTag(const QString &ctag);

    /**
     * Returns this collection CTag. The returned value will be empty
     * if no CTag was found.
     * @see https://github.com/apple/ccs-calendarserver/blob/master/doc/Extensions/caldav-ctag.txt
     */
    Q_REQUIRED_RESULT QString CTag() const;

    /**
     * Sets the @p url that identifies the collection.
     */
    void setUrl(const DavUrl &url);

    /**
     * Returns the URL that identifies the collection.
     */
    Q_REQUIRED_RESULT DavUrl url() const;

    /**
     * Sets the display @p name of the collection.
     */
    void setDisplayName(const QString &name);

    /**
     * Returns the display name of the collection.
     */
    Q_REQUIRED_RESULT QString displayName() const;

    /**
     * Sets the color for this collection
     */
    void setColor(const QColor &color);

    /**
     * Return the color of the collection, or an empty string if
     * none was provided by the backend.
     */
    Q_REQUIRED_RESULT QColor color() const;

    /**
     * Sets the possible content @p types of the collection.
     */
    void setContentTypes(ContentTypes types);

    /**
     * Returns the possible content types of the collection.
     */
    Q_REQUIRED_RESULT ContentTypes contentTypes() const;

    /**
     * Sets the privileges on this collection.
     */
    void setPrivileges(Privileges privs);

    /**
     * Returns the privileges on this collection.
     */
    Q_REQUIRED_RESULT Privileges privileges() const;

private:
    QSharedDataPointer<DavCollectionPrivate> d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(DavCollection::ContentTypes)

}

Q_DECLARE_TYPEINFO(KDAV::DavCollection, Q_MOVABLE_TYPE);

#endif
