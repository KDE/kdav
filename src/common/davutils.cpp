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

#include "davutils.h"
#include "davitem.h"
#include "davmanager.h"
#include "davprotocolbase.h"

#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QString>

QDomElement DavUtils::firstChildElementNS(const QDomElement &parent, const QString &namespaceUri, const QString &tagName)
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

QDomElement DavUtils::nextSiblingElementNS(const QDomElement &element, const QString &namespaceUri, const QString &tagName)
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

DavUtils::Privileges DavUtils::extractPrivileges(const QDomElement &element)
{
    Privileges final = None;
    QDomElement privElement = firstChildElementNS(element, QStringLiteral("DAV:"), QStringLiteral("privilege"));

    while (!privElement.isNull()) {
        QDomElement child = privElement.firstChildElement();

        while (!child.isNull()) {
            final |= parsePrivilege(child);
            child = child.nextSiblingElement();
        }

        privElement = DavUtils::nextSiblingElementNS(privElement, QStringLiteral("DAV:"), QStringLiteral("privilege"));
    }

    return final;
}

DavUtils::Privileges DavUtils::parsePrivilege(const QDomElement &element)
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
            final |= DavUtils::Read;
        } else if (privname == QLatin1String("write")) {
            final |= DavUtils::Write;
        } else if (privname == QLatin1String("write-properties")) {
            final |= DavUtils::WriteProperties;
        } else if (privname == QLatin1String("write-content")) {
            final |= DavUtils::WriteContent;
        } else if (privname == QLatin1String("unlock")) {
            final |= DavUtils::Unlock;
        } else if (privname == QLatin1String("read-acl")) {
            final |= DavUtils::ReadAcl;
        } else if (privname == QLatin1String("read-current-user-privilege-set")) {
            final |= DavUtils::ReadCurrentUserPrivilegeSet;
        } else if (privname == QLatin1String("write-acl")) {
            final |= DavUtils::WriteAcl;
        } else if (privname == QLatin1String("bind")) {
            final |= DavUtils::Bind;
        } else if (privname == QLatin1String("unbind")) {
            final |= DavUtils::Unbind;
        } else if (privname == QLatin1String("all")) {
            final |= DavUtils::All;
        }
    }

    return final;
}

DavUtils::DavUrl::DavUrl()
    : mProtocol(CalDav)
{
}

DavUtils::DavUrl::DavUrl(const QUrl &url, DavUtils::Protocol protocol)
    : mUrl(url), mProtocol(protocol)
{
}

void DavUtils::DavUrl::setUrl(const QUrl &url)
{
    mUrl = url;
}

QUrl DavUtils::DavUrl::url() const
{
    return mUrl;
}

void DavUtils::DavUrl::setProtocol(DavUtils::Protocol protocol)
{
    mProtocol = protocol;
}

DavUtils::Protocol DavUtils::DavUrl::protocol() const
{
    return mProtocol;
}

QLatin1String DavUtils::protocolName(DavUtils::Protocol protocol)
{
    QLatin1String protocolName("");

    switch (protocol) {
    case DavUtils::CalDav:
        protocolName = QLatin1String("CalDav");
        break;
    case DavUtils::CardDav:
        protocolName = QLatin1String("CardDav");
        break;
    case DavUtils::GroupDav:
        protocolName = QLatin1String("GroupDav");
        break;
    }

    return protocolName;
}

DavUtils::Protocol DavUtils::protocolByName(const QString &name)
{
    DavUtils::Protocol protocol = DavUtils::CalDav;

    if (name == QLatin1String("CalDav")) {
        protocol = DavUtils::CalDav;
    } else if (name == QLatin1String("CardDav")) {
        protocol = DavUtils::CardDav;
    } else if (name == QLatin1String("GroupDav")) {
        protocol = DavUtils::GroupDav;
    } else {
        qCritical() << "Unexpected protocol name : " << name;
    }

    return protocol;
}

QString DavUtils::createUniqueId()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch() / 1000;
    int r = qrand() % 1000;
    QString id = QLatin1String("R") + QString::number(r);
    QString uid = QString::number(time) + QLatin1String(".") + id;
    return uid;
}

QString DavUtils::contactsMimeType(DavUtils::Protocol protocol)
{
    QString ret;

    if (protocol == DavUtils::CardDav) {
        ret = QStringLiteral("text/vcard");
    } else if (protocol == DavUtils::GroupDav) {
        ret = QStringLiteral("text/x-vcard");
    }

    return ret;
}
