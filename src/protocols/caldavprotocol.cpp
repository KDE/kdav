/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "caldavprotocol_p.h"
#include "common/davcollection.h"
#include "common/utils_p.h"
#include "libkdav_debug.h"

#include <QColor>
#include <QDomDocument>
#include <QStringList>
#include <QUrl>
#include <QXmlStreamWriter>

using namespace KDAV;
using namespace KDAV::Xml;
using namespace Qt::StringLiterals;

class CaldavCollectionQueryBuilder : public XMLQueryBuilder
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
        propElement.appendChild(document.createElementNS(QStringLiteral("http://apple.com/ns/ical/"), QStringLiteral("calendar-color")));
        propElement.appendChild(document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("supported-calendar-component-set")));
        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("current-user-privilege-set")));
        propElement.appendChild(document.createElementNS(QStringLiteral("http://calendarserver.org/ns/"), QStringLiteral("getctag")));

        return document;
    }

    QString mimeType() const override
    {
        return QString();
    }
};

class CaldavListEventQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QString startTime = parameter(QStringLiteral("start")).toString();
        QString endTime = parameter(QStringLiteral("end")).toString();
        QDomDocument document;

        QDomElement queryElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-query"));
        document.appendChild(queryElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        queryElement.appendChild(propElement);

        QDomElement getetagElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag"));
        propElement.appendChild(getetagElement);

        QDomElement getRTypeElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("resourcetype"));
        propElement.appendChild(getRTypeElement);

        QDomElement filterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("filter"));
        queryElement.appendChild(filterElement);

        QDomElement compfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));

        QDomAttr nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VCALENDAR"));
        compfilterElement.setAttributeNode(nameAttribute);
        filterElement.appendChild(compfilterElement);

        QDomElement subcompfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));
        nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VEVENT"));
        subcompfilterElement.setAttributeNode(nameAttribute);

        if (!startTime.isEmpty() || !endTime.isEmpty()) {
            QDomElement timeRangeElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("time-range"));

            if (!startTime.isEmpty()) {
                QDomAttr startAttribute = document.createAttribute(QStringLiteral("start"));
                startAttribute.setValue(startTime);
                timeRangeElement.setAttributeNode(startAttribute);
            }

            if (!endTime.isEmpty()) {
                QDomAttr endAttribute = document.createAttribute(QStringLiteral("end"));
                endAttribute.setValue(endTime);
                timeRangeElement.setAttributeNode(endAttribute);
            }

            subcompfilterElement.appendChild(timeRangeElement);
        }

        compfilterElement.appendChild(subcompfilterElement);

        return document;
    }

    QString mimeType() const override
    {
        return QStringLiteral("application/x-vnd.akonadi.calendar.event");
    }
};

class CaldavListTodoQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QString startTime = parameter(QStringLiteral("start")).toString();
        QString endTime = parameter(QStringLiteral("end")).toString();
        QDomDocument document;

        QDomElement queryElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-query"));
        document.appendChild(queryElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        queryElement.appendChild(propElement);

        QDomElement getetagElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag"));
        propElement.appendChild(getetagElement);

        QDomElement getRTypeElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("resourcetype"));
        propElement.appendChild(getRTypeElement);

        QDomElement filterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("filter"));
        queryElement.appendChild(filterElement);

        QDomElement compfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));

        QDomAttr nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VCALENDAR"));
        compfilterElement.setAttributeNode(nameAttribute);
        filterElement.appendChild(compfilterElement);

        QDomElement subcompfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));
        nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VTODO"));
        subcompfilterElement.setAttributeNode(nameAttribute);

        if (!startTime.isEmpty() || !endTime.isEmpty()) {
            QDomElement timeRangeElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("time-range"));

            if (!startTime.isEmpty()) {
                QDomAttr startAttribute = document.createAttribute(QStringLiteral("start"));
                startAttribute.setValue(startTime);
                timeRangeElement.setAttributeNode(startAttribute);
            }

            if (!endTime.isEmpty()) {
                QDomAttr endAttribute = document.createAttribute(QStringLiteral("end"));
                endAttribute.setValue(endTime);
                timeRangeElement.setAttributeNode(endAttribute);
            }

            subcompfilterElement.appendChild(timeRangeElement);
        }

        compfilterElement.appendChild(subcompfilterElement);

        return document;
    }

    QString mimeType() const override
    {
        return QStringLiteral("application/x-vnd.akonadi.calendar.todo");
    }
};

class CaldavListJournalQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QString startTime = parameter(QStringLiteral("start")).toString();
        QString endTime = parameter(QStringLiteral("end")).toString();
        QDomDocument document;

        QDomElement queryElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-query"));
        document.appendChild(queryElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        queryElement.appendChild(propElement);

        QDomElement getetagElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag"));
        propElement.appendChild(getetagElement);

        QDomElement getRTypeElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("resourcetype"));
        propElement.appendChild(getRTypeElement);

        QDomElement filterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("filter"));
        queryElement.appendChild(filterElement);

        QDomElement compfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));

        QDomAttr nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VCALENDAR"));
        compfilterElement.setAttributeNode(nameAttribute);
        filterElement.appendChild(compfilterElement);

        QDomElement subcompfilterElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp-filter"));
        nameAttribute = document.createAttribute(QStringLiteral("name"));
        nameAttribute.setValue(QStringLiteral("VJOURNAL"));
        subcompfilterElement.setAttributeNode(nameAttribute);

        if (!startTime.isEmpty() || !endTime.isEmpty()) {
            QDomElement timeRangeElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("time-range"));

            if (!startTime.isEmpty()) {
                QDomAttr startAttribute = document.createAttribute(QStringLiteral("start"));
                startAttribute.setValue(startTime);
                timeRangeElement.setAttributeNode(startAttribute);
            }

            if (!endTime.isEmpty()) {
                QDomAttr endAttribute = document.createAttribute(QStringLiteral("end"));
                endAttribute.setValue(endTime);
                timeRangeElement.setAttributeNode(endAttribute);
            }

            subcompfilterElement.appendChild(timeRangeElement);
        }

        compfilterElement.appendChild(subcompfilterElement);

        return document;
    }

    QString mimeType() const override
    {
        return QStringLiteral("application/x-vnd.akonadi.calendar.journal");
    }
};

class CaldavMultigetQueryBuilder : public XMLQueryBuilder
{
public:
    QDomDocument buildQuery() const override
    {
        QDomDocument document;
        const QStringList urls = parameter(QStringLiteral("urls")).toStringList();
        if (urls.isEmpty()) {
            return document;
        }

        QDomElement multigetElement = document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-multiget"));
        document.appendChild(multigetElement);

        QDomElement propElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("prop"));
        multigetElement.appendChild(propElement);

        propElement.appendChild(document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("getetag")));
        propElement.appendChild(document.createElementNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar-data")));

        for (const QString &url : urls) {
            QDomElement hrefElement = document.createElementNS(QStringLiteral("DAV:"), QStringLiteral("href"));
            const QUrl pathUrl = QUrl::fromUserInput(url);
            qCDebug(KDAV_LOG) << pathUrl.toString() << "->" << pathUrl.path(QUrl::FullyEncoded);
            const QDomText textNode = document.createTextNode(pathUrl.path(QUrl::FullyEncoded));
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

CaldavProtocol::CaldavProtocol()
{
}

bool CaldavProtocol::supportsPrincipals() const
{
    return true;
}

bool CaldavProtocol::useReport() const
{
    return true;
}

bool CaldavProtocol::useMultiget() const
{
    return true;
}

QString CaldavProtocol::principalHomeSet() const
{
    return QStringLiteral("calendar-home-set");
}

QString CaldavProtocol::principalHomeSetNS() const
{
    return QStringLiteral("urn:ietf:params:xml:ns:caldav");
}

XMLQueryBuilder::Ptr CaldavProtocol::collectionsQuery() const
{
    return XMLQueryBuilder::Ptr(new CaldavCollectionQueryBuilder());
}

bool CaldavProtocol::containsCollection(const QDomElement &propElem) const
{
    return !propElem.elementsByTagNameNS(QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("calendar")).isEmpty();
}

QList<XMLQueryBuilder::Ptr> CaldavProtocol::itemsQueries() const
{
    QList<XMLQueryBuilder::Ptr> ret;
    ret << XMLQueryBuilder::Ptr(new CaldavListEventQueryBuilder());
    ret << XMLQueryBuilder::Ptr(new CaldavListTodoQueryBuilder());
    ret << XMLQueryBuilder::Ptr(new CaldavListJournalQueryBuilder());
    return ret;
}

XMLQueryBuilder::Ptr CaldavProtocol::itemsReportQuery(const QStringList &urls) const
{
    XMLQueryBuilder::Ptr ret(new CaldavMultigetQueryBuilder());
    ret->setParameter(QStringLiteral("urls"), urls);
    return ret;
}

QString CaldavProtocol::responseNamespace() const
{
    return QStringLiteral("urn:ietf:params:xml:ns:caldav");
}

QString CaldavProtocol::dataTagName() const
{
    return QStringLiteral("calendar-data");
}

DavCollection::ContentTypes CaldavProtocol::collectionContentTypes(const QDomElement &propstatElement) const
{
    /*
     * Extract the content type information from a propstat like the following
     *   <propstat xmlns="DAV:">
     *     <prop xmlns="DAV:">
     *       <C:supported-calendar-component-set xmlns:C="urn:ietf:params:xml:ns:caldav">
     *         <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VEVENT"/>
     *         <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VTODO"/>
     *         <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VJOURNAL"/>
     *         <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VTIMEZONE"/>
     *         <C:comp xmlns:C="urn:ietf:params:xml:ns:caldav" name="VFREEBUSY"/>
     *       </C:supported-calendar-component-set>
     *       <resourcetype xmlns="DAV:">
     *         <collection xmlns="DAV:"/>
     *         <C:calendar xmlns:C="urn:ietf:params:xml:ns:caldav"/>
     *         <C:schedule-calendar xmlns:C="urn:ietf:params:xml:ns:caldav"/>
     *       </resourcetype>
     *       <displayname xmlns="DAV:">Test1 User</displayname>
     *     </prop>
     *     <status xmlns="DAV:">HTTP/1.1 200 OK</status>
     *   </propstat>
     */

    const QDomElement propElement = Utils::firstChildElementNS(propstatElement, QStringLiteral("DAV:"), QStringLiteral("prop"));
    const QDomElement supportedcomponentElement = Utils::firstChildElementNS(propElement, //
                                                                             QStringLiteral("urn:ietf:params:xml:ns:caldav"),
                                                                             QStringLiteral("supported-calendar-component-set"));

    DavCollection::ContentTypes contentTypes;
    QDomElement compElement = Utils::firstChildElementNS(supportedcomponentElement, QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp"));

    /*
     * Assign the content-type if the server didn't return anything.
     * According to RFC4791, §5.2.3:
     * In the absence of this property, the server MUST accept all
     * component types, and the client can assume that all component
     * types are accepted.
     */
    if (compElement.isNull()) {
        contentTypes |= DavCollection::Calendar;
        contentTypes |= DavCollection::Events;
        contentTypes |= DavCollection::Todos;
        contentTypes |= DavCollection::FreeBusy;
        contentTypes |= DavCollection::Journal;
        contentTypes |= DavCollection::Timezone;
    }

    while (!compElement.isNull()) {
        const QString type = compElement.attribute(QStringLiteral("name")).toLower();
        if (type == QLatin1String("vcalendar")) {
            contentTypes |= DavCollection::Calendar;
        } else if (type == QLatin1String("vevent")) {
            contentTypes |= DavCollection::Events;
        } else if (type == QLatin1String("vtodo")) {
            contentTypes |= DavCollection::Todos;
        } else if (type == QLatin1String("vfreebusy")) {
            contentTypes |= DavCollection::FreeBusy;
        } else if (type == QLatin1String("vjournal")) {
            contentTypes |= DavCollection::Journal;
        } else if (type == QLatin1String("vtimezone")) {
            contentTypes |= DavCollection::Timezone;
        }

        compElement = Utils::nextSiblingElementNS(compElement, QStringLiteral("urn:ietf:params:xml:ns:caldav"), QStringLiteral("comp"));
    }

    return contentTypes;
}

void CaldavProtocol::writeMkCol(QXmlStreamWriter &writer, KDAV::DavCollection &collection) const
{
    writer.writeStartDocument();

    writer.writeNamespace(davNS, "D"_L1);
    writer.writeNamespace(icalNS, "I"_L1);
    writer.writeNamespace(caldavNS, "C"_L1);
    writer.writeStartElement(davNS, "mkcol"_L1);
    writer.writeStartElement(davNS, "set"_L1);

    writer.writeStartElement(davNS, "prop"_L1);

    writer.writeStartElement(davNS, "resourcetype"_L1);
    writer.writeEmptyElement(davNS, "collection"_L1);
    writer.writeEmptyElement(caldavNS, "calendar"_L1);
    writer.writeEndElement(); // resourcetype

    writer.writeTextElement(davNS, "displayname"_L1, collection.displayName());

    if (collection.color().isValid()) {
        // If it's ARGB, we need to print it in RGBA format
        const auto color = [&] -> QString {
            if (collection.color().alpha() == 255) {
                return collection.color().name(QColor::HexRgb);
            }
            const auto color = collection.color().name(QColor::HexArgb);
            return QStringLiteral("#") + color.mid(3, 6) + color.mid(1, 2);
        }();
        writer.writeTextElement(icalNS, "calendar-color"_L1, color);
    }

    writer.writeStartElement(caldavNS, "supported-calendar-component-set"_L1);
    if (collection.contentTypes() & DavCollection::Calendar) {
        writer.writeEmptyElement(caldavNS, "comp"_L1);
        writer.writeAttribute("name"_L1, "VCALENDAR"_L1);
    }
    if (collection.contentTypes() & DavCollection::Events) {
        writer.writeEmptyElement(caldavNS, "comp"_L1);
        writer.writeAttribute("name"_L1, "VEVENT"_L1);
    }
    if (collection.contentTypes() & DavCollection::Todos) {
        writer.writeEmptyElement(caldavNS, "comp"_L1);
        writer.writeAttribute("name"_L1, "VTODO"_L1);
    }
    if (collection.contentTypes() & DavCollection::FreeBusy) {
        writer.writeEmptyElement(caldavNS, "comp"_L1);
        writer.writeAttribute("name"_L1, "VFREEBUSY"_L1);
    }
    if (collection.contentTypes() & DavCollection::Journal) {
        writer.writeEmptyElement(caldavNS, "comp"_L1);
        writer.writeAttribute("name"_L1, "VJOURNAL"_L1);
    }
    writer.writeEndElement();

    if (!collection.timeZone().isEmpty()) {
        writer.writeTextElement(caldavNS, "calendar-timezone"_L1, QString::fromUtf8(collection.timeZone()));
    }
    writer.writeEndElement(); // prop
    writer.writeEndElement(); // set
    writer.writeEndElement(); // mkcol
}
