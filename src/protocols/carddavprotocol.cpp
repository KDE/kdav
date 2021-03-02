/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "carddavprotocol_p.h"

#include <QDomDocument>
#include <QStringList>
#include <QUrl>

using namespace KDAV;

class CarddavCollectionQueryBuilder : public XMLQueryBuilder
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
        propElement.appendChild(document.createElementNS(QStringLiteral("http://calendarserver.org/ns/"), QStringLiteral("getctag")));

        return document;
    }

    QString mimeType() const override
    {
        return QString();
    }
};

class CarddavListItemsQueryBuilder : public XMLQueryBuilder
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
        return QStringLiteral("text/directory");
    }
};

class CarddavMultigetQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QDomDocument document;
        const QStringList urls = parameter(QStringLiteral("urls")).toStringList();
        if (urls.isEmpty()) {
            return document;
        }

        QDomElement multigetElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:carddav"), QStringLiteral("addressbook-multiget"));
        document.appendChild(multigetElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        multigetElement.appendChild(propElement);

        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag")));
        QDomElement addressDataElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:carddav"), QStringLiteral("address-data"));
        addressDataElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("allprop")));
        propElement.appendChild(addressDataElement);

        for (const QString &url : urls) {
            QDomElement hrefElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("href"));
            const QUrl pathUrl = QUrl::fromUserInput(url);
            const QDomText textNode = document.createTextNode(pathUrl.toString());
            hrefElement.appendChild(textNode);

            multigetElement.appendChild(hrefElement);
        }

        return document;
    }

    QString mimeType() const override
    {
        return QString();
    }
};

CarddavProtocol::CarddavProtocol()
{
}

bool CarddavProtocol::supportsPrincipals() const
{
    return true;
}

bool CarddavProtocol::useReport() const
{
    return false;
}

bool CarddavProtocol::useMultiget() const
{
    return true;
}

QString CarddavProtocol::principalHomeSet() const
{
    return QStringLiteral("addressbook-home-set");
}

QString CarddavProtocol::principalHomeSetNS() const
{
    return QStringLiteral("urn:ietf:params:xml:ns:carddav");
}

XMLQueryBuilder::Ptr CarddavProtocol::collectionsQuery() const
{
    return XMLQueryBuilder::Ptr(new CarddavCollectionQueryBuilder());
}

QString CarddavProtocol::collectionsXQuery() const
{
    // clang-format off
    const QString query(QStringLiteral("//*[local-name()='addressbook' and namespace-uri()='urn:ietf:params:xml:ns:carddav']/ancestor::*[local-name()='response' and namespace-uri()='DAV:']"));

    return query;
    // clang-format on
}

QVector<XMLQueryBuilder::Ptr> CarddavProtocol::itemsQueries() const
{
    QVector<XMLQueryBuilder::Ptr> ret;
    ret << XMLQueryBuilder::Ptr(new CarddavListItemsQueryBuilder());
    return ret;
}

XMLQueryBuilder::Ptr CarddavProtocol::itemsReportQuery(const QStringList &urls) const
{
    XMLQueryBuilder::Ptr ret(new CarddavMultigetQueryBuilder());
    ret->setParameter(QStringLiteral("urls"), urls);
    return ret;
}

QString CarddavProtocol::responseNamespace() const
{
    return QStringLiteral("urn:ietf:params:xml:ns:carddav");
}

QString CarddavProtocol::dataTagName() const
{
    return QStringLiteral("address-data");
}

DavCollection::ContentTypes CarddavProtocol::collectionContentTypes(const QDomElement &) const
{
    return DavCollection::Contacts;
}
