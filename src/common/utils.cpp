/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "protocolinfo.h"
#include "utils_p.h"

#include "enums.h"

#include "davitem.h"
#include "davmanager_p.h"
#include "davprotocolbase_p.h"

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
