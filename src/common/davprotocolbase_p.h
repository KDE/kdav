/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPROTOCOLBASE_H
#define KDAV_DAVPROTOCOLBASE_H

#include "kdav_export.h"

#include "davcollection.h"

#include <QDomDocument>
#include <QMap>
#include <QSharedPointer>
#include <QVariant>

namespace KDAV
{
/**
 * @short Base class for XML query builders
 */
class XMLQueryBuilder
{
public:
    typedef QSharedPointer<XMLQueryBuilder> Ptr;

    virtual ~XMLQueryBuilder();

    virtual QDomDocument buildQuery() const = 0;
    virtual QString mimeType() const = 0;

    void setParameter(const QString &key, const QVariant &value);
    QVariant parameter(const QString &key) const;

private:
    QMap<QString, QVariant> mParameters;
};

/**
 * @short Base class for various DAV groupware dialects.
 *
 * This class provides an interface to query the DAV dialect
 * specific features and abstract them.
 *
 * The functionality is implemented in:
 *   @li CaldavProtocol
 *   @li CarddavProtocol
 *   @li GroupdavProtocol
 */
class DavProtocolBase
{
public:
    /**
     * Destroys the DAV protocol base.
     */
    virtual ~DavProtocolBase();

    /**
     * Returns whether the DAV protocol dialect supports principal
     * queries. If true, it must return the home set it provides
     * access to with principalHomeSet() and the home set namespace
     * with principalHomeSetNS();
     */
    virtual bool supportsPrincipals() const = 0;

    /**
     * Returns whether the DAV protocol dialect supports the REPORT
     * command to query all resources of a collection.
     * If not, PROPFIND command will be used instead.
     */
    virtual bool useReport() const = 0;

    /**
     * Returns whether the DAV protocol dialect supports the MULTIGET command.
     *
     * If MULTIGET is supported, the content of all DAV resources
     * can be fetched in Akonadi::ResourceBase::retrieveItems() already and
     * there is no need to call Akonadi::ResourceBase::retrieveItem() for every single
     * DAV resource.
     *
     * Protocols that have MULTIGET capabilities must inherit from
     * DavMultigetProtocol instead of this class.
     */
    virtual bool useMultiget() const = 0;

    /**
     * Returns the home set that this protocol supports.
     */
    virtual QString principalHomeSet() const;

    /**
     * Returns the namespace of the home set.
     */
    virtual QString principalHomeSetNS() const;

    /**
     * Returns the XML document that represents the DAV query to
     * list all available DAV collections.
     */
    virtual XMLQueryBuilder::Ptr collectionsQuery() const = 0;

    /**
     * Returns @c true if the given <prop> element of a multistatus response contains a
     * valid collection for this protocol.
     */
    virtual bool containsCollection(const QDomElement &propElem) const = 0;

    /**
     * Returns a list of XML documents that represent DAV queries to
     * list all available DAV resources inside a specific DAV collection.
     */
    virtual QVector<XMLQueryBuilder::Ptr> itemsQueries() const = 0;

    /**
     * Returns the possible content types for the collection that
     * is described by the passed @p propstat element of a PROPFIND result.
     */
    virtual DavCollection::ContentTypes collectionContentTypes(const QDomElement &propstat) const = 0;
};
}

#endif
