// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davxml_p.h"

#include "davcollection.h"

#include <QColor>
#include <QXmlStreamWriter>

using namespace Qt::StringLiterals;

namespace KDAV
{
namespace Xml
{

std::optional<Protocol> protocolFromCollection(DavCollection &collection)
{
    const auto isCalDav =
        collection.contentTypes() & (DavCollection::Events | DavCollection::Todos | DavCollection::Journal | DavCollection::Calendar | DavCollection::FreeBusy);
    if (isCalDav) {
        return Protocol::CalDav;
    }

    if (collection.contentTypes() & DavCollection::Contacts) {
        return Protocol::CardDav;
    }

    return std::nullopt;
}

void writeMkCol(QXmlStreamWriter &writer, DavCollection &collection)
{
    const auto protocol = protocolFromCollection(collection);

    writer.writeStartDocument();

    writer.writeNamespace(davNS, "D"_L1);
    if (protocol && *protocol == Protocol::CalDav) {
        writer.writeNamespace(icalNS, "I"_L1);
        writer.writeNamespace(caldavNS, "C"_L1);
    } else if (protocol && *protocol == Protocol::CardDav) {
        writer.writeNamespace(carddavNS, "C"_L1);
    }

    writer.writeStartElement(davNS, "mkcol"_L1);
    writer.writeStartElement(davNS, "set"_L1);
    Xml::writeCollectionProperties(writer, collection, protocol);
    writer.writeEndElement(); // set
    writer.writeEndElement(); // mkcol
}

void writeCollectionProperties(QXmlStreamWriter &writer, DavCollection &collection, std::optional<Protocol> protocol)
{
    writer.writeStartElement(davNS, "prop"_L1);

    writer.writeStartElement(davNS, "resourcetype"_L1);
    writer.writeEmptyElement(davNS, "collection"_L1);
    if (protocol && *protocol == Protocol::CalDav) {
        writer.writeEmptyElement(caldavNS, "calendar"_L1);
    } else if (protocol && *protocol == Protocol::CardDav) {
        writer.writeEmptyElement(carddavNS, "addressbook"_L1);
    }
    writer.writeEndElement(); // resourcetype

    writer.writeTextElement(davNS, "displayname"_L1, collection.displayName());

    if (protocol == Protocol::CalDav) {
        if (collection.color().isValid()) {
            const QString color = collection.color().name(QColor::HexArgb);
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
    }
    writer.writeEndElement(); // prop
}

} // end namespace Xml
} // end namespace KDAV
