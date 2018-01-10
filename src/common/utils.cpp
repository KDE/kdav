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

#include "utils.h"

#include "enums.h"

#include "davitem.h"
#include "davmanager.h"
#include "davprotocolbase.h"

#include <QDateTime>
#include <QString>

#include "libkdav_debug.h"

using namespace KDAV;

QDomElement Utils::firstChildElementNS(const QDomElement &parent, const QString &namespaceUri, const QString &tagName)
{
    for (QDomNode child = parent.firstChild(); !child.isNull(); child = child.nextSibling()) {
        if (child.isElement()) {
            const QDomElement elt = child.toElement();
            if (tagName.isEmpty() || (elt.tagName() == tagName && elt.namespaceURI() == namespaceUri)) {
                return elt;
            }
        }
    }

    return QDomElement();
}

QDomElement Utils::nextSiblingElementNS(const QDomElement &element, const QString &namespaceUri, const QString &tagName)
{
    for (QDomNode sib = element.nextSibling(); !sib.isNull(); sib = sib.nextSibling()) {
        if (sib.isElement()) {
            const QDomElement elt = sib.toElement();
            if (tagName.isEmpty() || (elt.tagName() == tagName && elt.namespaceURI() == namespaceUri)) {
                return elt;
            }
        }
    }

    return QDomElement();
}

Privileges Utils::extractPrivileges(const QDomElement &element)
{
    Privileges final = None;
    QDomElement privElement = firstChildElementNS(element, QStringLiteral("DAV:"), QStringLiteral("privilege"));

    while (!privElement.isNull()) {
        QDomElement child = privElement.firstChildElement();

        while (!child.isNull()) {
            final |= parsePrivilege(child);
            child = child.nextSiblingElement();
        }

        privElement = Utils::nextSiblingElementNS(privElement, QStringLiteral("DAV:"), QStringLiteral("privilege"));
    }

    return final;
}

Privileges Utils::parsePrivilege(const QDomElement &element)
{
    Privileges final = None;

    if (!element.childNodes().isEmpty()) {
        // This is an aggregate privilege, parse each of its children
        QDomElement child = element.firstChildElement();
        while (!child.isNull()) {
            final |= parsePrivilege(child);
            child = child.nextSiblingElement();
        }
    } else {
        // This is a normal privilege
        const QString privname = element.localName();

        if (privname == QLatin1String("read")) {
            final |= KDAV::Read;
        } else if (privname == QLatin1String("write")) {
            final |= KDAV::Write;
        } else if (privname == QLatin1String("write-properties")) {
            final |= KDAV::WriteProperties;
        } else if (privname == QLatin1String("write-content")) {
            final |= KDAV::WriteContent;
        } else if (privname == QLatin1String("unlock")) {
            final |= KDAV::Unlock;
        } else if (privname == QLatin1String("read-acl")) {
            final |= KDAV::ReadAcl;
        } else if (privname == QLatin1String("read-current-user-privilege-set")) {
            final |= KDAV::ReadCurrentUserPrivilegeSet;
        } else if (privname == QLatin1String("write-acl")) {
            final |= KDAV::WriteAcl;
        } else if (privname == QLatin1String("bind")) {
            final |= KDAV::Bind;
        } else if (privname == QLatin1String("unbind")) {
            final |= KDAV::Unbind;
        } else if (privname == QLatin1String("all")) {
            final |= KDAV::All;
        }
    }

    return final;
}

QLatin1String Utils::protocolName(Protocol protocol)
{
    QLatin1String protocolName("");

    switch (protocol) {
    case KDAV::CalDav:
        protocolName = QLatin1String("CalDav");
        break;
    case KDAV::CardDav:
        protocolName = QLatin1String("CardDav");
        break;
    case KDAV::GroupDav:
        protocolName = QLatin1String("GroupDav");
        break;
    }

    return protocolName;
}

Protocol Utils::protocolByName(const QString &name)
{
    Protocol protocol = KDAV::CalDav;

    if (name == QLatin1String("CalDav")) {
        protocol = KDAV::CalDav;
    } else if (name == QLatin1String("CardDav")) {
        protocol = KDAV::CardDav;
    } else if (name == QLatin1String("GroupDav")) {
        protocol = KDAV::GroupDav;
    } else {
        qCCritical(KDAV_LOG) << "Unexpected protocol name : " << name;
    }

    return protocol;
}

QString Utils::createUniqueId()
{
    const qint64 time = QDateTime::currentMSecsSinceEpoch() / 1000;
    const int r = qrand() % 1000;
    const QString id = QLatin1Char('R') + QString::number(r);
    const QString uid = QString::number(time) + QLatin1Char('.') + id;
    return uid;
}

QString Utils::contactsMimeType(Protocol protocol)
{
    QString ret;

    if (protocol == KDAV::CardDav) {
        ret = QStringLiteral("text/vcard");
    } else if (protocol == KDAV::GroupDav) {
        ret = QStringLiteral("text/x-vcard");
    }

    return ret;
}
