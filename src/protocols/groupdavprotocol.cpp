/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "groupdavprotocol_p.h"

#include "common/utils_p.h"

#include <QDomDocument>

using namespace KDAV;

class GroupdavCollectionQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QDomDocument document;

        QDomElement propfindElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propfind"));
        document.appendChild(propfindElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        propfindElement.appendChild(propElement);

        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("displayname")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("resourcetype")));

        return document;
    }

    QString mimeType() const override
    {
        return QString();
    }
};

class GroupdavItemQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QDomDocument document;

        QDomElement propfindElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("propfind"));
        document.appendChild(propfindElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        propfindElement.appendChild(propElement);

        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("displayname")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("resourcetype")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag")));

        return document;
    }

    QString mimeType() const override
    {
        return QString();
    }
};

GroupdavProtocol::GroupdavProtocol()
{
}

bool GroupdavProtocol::supportsPrincipals() const
{
    return false;
}

bool GroupdavProtocol::useReport() const
{
    return false;
}

bool GroupdavProtocol::useMultiget() const
{
    return false;
}

XMLQueryBuilder::Ptr GroupdavProtocol::collectionsQuery() const
{
    return XMLQueryBuilder::Ptr(new GroupdavCollectionQueryBuilder());
}

bool GroupdavProtocol::containsCollection(const QDomElement &propElem) const
{
    return !propElem.elementsByTagNameNS(QStringLiteral("http://groupdav.org/"), QStringLiteral("vevent-collection")).isEmpty()
        || !propElem.elementsByTagNameNS(QStringLiteral("http://groupdav.org/"), QStringLiteral("vtodo-collection")).isEmpty()
        || !propElem.elementsByTagNameNS(QStringLiteral("http://groupdav.org/"), QStringLiteral("vcard-collection")).isEmpty();
}

QVector<XMLQueryBuilder::Ptr> GroupdavProtocol::itemsQueries() const
{
    QVector<XMLQueryBuilder::Ptr> ret;
    ret << XMLQueryBuilder::Ptr(new GroupdavItemQueryBuilder());
    return ret;
}

DavCollection::ContentTypes GroupdavProtocol::collectionContentTypes(const QDomElement &propstatElement) const
{
    /*
     * Extract the content type information from a propstat like the following
     *
     *  <propstat>
     *    <status>HTTP/1.1 200 OK</status>
     *    <prop>
     *      <displayname>Tasks</displayname>
     *      <resourcetype>
     *        <collection/>
     *        <G:vtodo-collection xmlns:G="http://groupdav.org/"/>
     *      </resourcetype>
     *      <getlastmodified>Sat, 30 Jan 2010 17:52:41 -0100</getlastmodified>
     *    </prop>
     *  </propstat>
     */

    const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
    const QDomElement resourcetypeElement = Utils::firstChildElementNS(propElement, QStringLiteral("DAV:"), QStringLiteral("resourcetype"));

    DavCollection::ContentTypes contentTypes;

    if (!Utils::firstChildElementNS(resourcetypeElement, QStringLiteral("http://groupdav.org/"), QStringLiteral("vevent-collection")).isNull()) {
        contentTypes |= DavCollection::Events;
    }

    if (!Utils::firstChildElementNS(resourcetypeElement, QStringLiteral("http://groupdav.org/"), QStringLiteral("vtodo-collection")).isNull()) {
        contentTypes |= DavCollection::Todos;
    }

    if (!Utils::firstChildElementNS(resourcetypeElement, QStringLiteral("http://groupdav.org/"), QStringLiteral("vcard-collection")).isNull()) {
        contentTypes |= DavCollection::Contacts;
    }

    return contentTypes;
}
