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

#include "davutils-akonadi.h"
#include "davitem.h"
#include "resource/davprotocolattribute.h"

#include <AkonadiCore/Collection>
#include <AkonadiCore/Item>
#include <KContacts/Addressee>
#include <KContacts/VCardConverter>
#include <KCalCore/ICalFormat>
#include <KCalCore/Incidence>
#include <KCalCore/MemoryCalendar>

#include <KLocalizedString>

#include <QtCore/QByteArray>
#include <QtCore/QString>

using namespace KDAV;

typedef QSharedPointer<KCalCore::Incidence> IncidencePtr;

QString Utils::translatedProtocolName(Utils::Protocol protocol)
{
    QString protocolName;

    switch (protocol) {
    case Utils::CalDav:
        protocolName = i18n("CalDav");
        break;
    case Utils::CardDav:
        protocolName = i18n("CardDav");
        break;
    case Utils::GroupDav:
        protocolName = i18n("GroupDav");
        break;
    }

    return protocolName;
}

Utils::Protocol Utils::protocolByTranslatedName(const QString &name)
{
    Utils::Protocol protocol = Utils::CalDav;

    if (name == i18n("CalDav")) {
        protocol = Utils::CalDav;
    } else if (name == i18n("CardDav")) {
        protocol = Utils::CardDav;
    } else if (name == i18n("GroupDav")) {
        protocol = Utils::GroupDav;
    }

    return protocol;
}

DavItem Utils::createDavItem(const Akonadi::Item &item, const Akonadi::Collection &collection, const Akonadi::Item::List &dependentItems)
{
    QByteArray rawData;
    QString mimeType;
    QUrl url;
    DavItem davItem;
    const QString basePath = collection.remoteId();

    if (item.hasPayload<KContacts::Addressee>()) {
        const KContacts::Addressee contact = item.payload<KContacts::Addressee>();
        const QString fileName = createUniqueId();

        url = QUrl(basePath + fileName + QLatin1String(".vcf"));

        const DavProtocolAttribute *protoAttr = collection.attribute<DavProtocolAttribute>();
        if (protoAttr) {
            mimeType = Utils::contactsMimeType(Utils::Protocol(protoAttr->davProtocol()));
        } else {
            mimeType = KContacts::Addressee::mimeType();
        }

        KContacts::VCardConverter converter;
        // rawData is already UTF-8
        rawData = converter.exportVCard(contact, KContacts::VCardConverter::v3_0);
    } else if (item.hasPayload<IncidencePtr>()) {
        const KCalCore::MemoryCalendar::Ptr calendar(new KCalCore::MemoryCalendar(KDateTime::LocalZone));
        calendar->addIncidence(item.payload<IncidencePtr>());
        foreach (const Akonadi::Item &dependentItem, dependentItems) {
            calendar->addIncidence(dependentItem.payload<IncidencePtr>());
        }

        const QString fileName = createUniqueId();

        url = QUrl(basePath + fileName + QLatin1String(".ics"));
        mimeType = QStringLiteral("text/calendar");

        KCalCore::ICalFormat formatter;
        rawData = formatter.toString(calendar, QString()).toUtf8();
    }

    davItem.setContentType(mimeType);
    davItem.setData(rawData);
    davItem.setUrl(url.toDisplayString());
    davItem.setEtag(item.remoteRevision());

    return davItem;
}

bool Utils::parseDavData(const DavItem &source, Akonadi::Item &target, Akonadi::Item::List &extraItems)
{
    const QString data = QString::fromUtf8(source.data());

    if (target.mimeType() == KContacts::Addressee::mimeType()) {
        KContacts::VCardConverter converter;
        const KContacts::Addressee contact = converter.parseVCard(source.data());

        if (contact.isEmpty()) {
            return false;
        }

        target.setPayloadFromData(source.data());
    } else {
        KCalCore::ICalFormat formatter;
        const KCalCore::MemoryCalendar::Ptr calendar(new KCalCore::MemoryCalendar(KDateTime::LocalZone));
        formatter.fromString(calendar, data);
        KCalCore::Incidence::List incidences = calendar->incidences();

        if (incidences.isEmpty()) {
            return false;
        }

        // All items must have the same uid in a single object.
        // Find the main VEVENT (if that's indeed what we have,
        // could be a VTODO or a VJOURNAL but that doesn't matter)
        // and then apply the recurrence exceptions
        IncidencePtr mainIncidence;
        KCalCore::Incidence::List exceptions;

        foreach (const IncidencePtr &incidence, incidences) {
            if (incidence->hasRecurrenceId()) {
                qCDebug(KDAV_LOG) << "Exception found with ID" << incidence->instanceIdentifier();
                exceptions << incidence;
            } else {
                mainIncidence = incidence;
            }
        }

        if (!mainIncidence) {
            return false;
        }

        foreach (const IncidencePtr &exception, exceptions) {
            if (exception->status() == KCalCore::Incidence::StatusCanceled) {
                KDateTime exDateTime = exception->recurrenceId();
                mainIncidence->recurrence()->addExDateTime(exDateTime);
            } else {
                // The exception remote id will contain a fragment pointing to
                // its instance identifier to distinguish it from the main
                // event.
                QString rid = target.remoteId() + QLatin1String("#") + exception->instanceIdentifier();
                qCDebug(KDAV_LOG) << "Extra incidence at" << rid;
                Akonadi::Item extraItem = target;
                extraItem.setRemoteId(rid);
                extraItem.setRemoteRevision(source.etag());
                extraItem.setMimeType(exception->mimeType());
                extraItem.setPayload<IncidencePtr>(exception);
                extraItems << extraItem;
            }
        }

        target.setPayload<IncidencePtr>(mainIncidence);
        // fix mime type for CalDAV collections
        target.setMimeType(mainIncidence->mimeType());

        /*
        foreach ( const IncidencePtr &incidence, incidences ) {
          QString rid = item.remoteId() + QLatin1String( "#" ) + incidence->instanceIdentifier();
          Akonadi::Item extraItem = item;
          extraItem.setRemoteId( rid );
          extraItem.setRemoteRevision( davItem.etag() );
          extraItem.setMimeType( incidence->mimeType() );
          extraItem.setPayload<IncidencePtr>( incidence );
          items << extraItem;
        }
        */
    }

    return true;
}
