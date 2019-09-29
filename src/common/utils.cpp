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

#include "utils_p.h"
#include "protocolinfo.h"

#include "enums.h"

#include "davitem.h"
#include "davmanager_p.h"
#include "davprotocolbase_p.h"

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
