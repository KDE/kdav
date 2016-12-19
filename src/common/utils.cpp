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

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QString>

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

Utils::Privileges Utils::extractPrivileges(const QDomElement &element)
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

Utils::Privileges Utils::parsePrivilege(const QDomElement &element)
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
            final |= Utils::Read;
        } else if (privname == QLatin1String("write")) {
            final |= Utils::Write;
        } else if (privname == QLatin1String("write-properties")) {
            final |= Utils::WriteProperties;
        } else if (privname == QLatin1String("write-content")) {
            final |= Utils::WriteContent;
        } else if (privname == QLatin1String("unlock")) {
            final |= Utils::Unlock;
        } else if (privname == QLatin1String("read-acl")) {
            final |= Utils::ReadAcl;
        } else if (privname == QLatin1String("read-current-user-privilege-set")) {
            final |= Utils::ReadCurrentUserPrivilegeSet;
        } else if (privname == QLatin1String("write-acl")) {
            final |= Utils::WriteAcl;
        } else if (privname == QLatin1String("bind")) {
            final |= Utils::Bind;
        } else if (privname == QLatin1String("unbind")) {
            final |= Utils::Unbind;
        } else if (privname == QLatin1String("all")) {
            final |= Utils::All;
        }
    }

    return final;
}

QLatin1String Utils::protocolName(Utils::Protocol protocol)
{
    QLatin1String protocolName("");

    switch (protocol) {
    case Utils::CalDav:
        protocolName = QLatin1String("CalDav");
        break;
    case Utils::CardDav:
        protocolName = QLatin1String("CardDav");
        break;
    case Utils::GroupDav:
        protocolName = QLatin1String("GroupDav");
        break;
    }

    return protocolName;
}

Utils::Protocol Utils::protocolByName(const QString &name)
{
    Utils::Protocol protocol = Utils::CalDav;

    if (name == QLatin1String("CalDav")) {
        protocol = Utils::CalDav;
    } else if (name == QLatin1String("CardDav")) {
        protocol = Utils::CardDav;
    } else if (name == QLatin1String("GroupDav")) {
        protocol = Utils::GroupDav;
    } else {
        qCCritical(KDAV_LOG) << "Unexpected protocol name : " << name;
    }

    return protocol;
}

QString Utils::createUniqueId()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch() / 1000;
    int r = qrand() % 1000;
    QString id = QLatin1String("R") + QString::number(r);
    QString uid = QString::number(time) + QLatin1String(".") + id;
    return uid;
}

QString Utils::contactsMimeType(Utils::Protocol protocol)
{
    QString ret;

    if (protocol == Utils::CardDav) {
        ret = QStringLiteral("text/vcard");
    } else if (protocol == Utils::GroupDav) {
        ret = QStringLiteral("text/x-vcard");
    }

    return ret;
}
