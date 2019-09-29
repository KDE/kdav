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

#include "protocolinfo.h"
#include "davmanager_p.h"
#include "davprotocolbase_p.h"
#include "libkdav_debug.h"

using namespace KDAV;

bool ProtocolInfo::useMultiget(KDAV::Protocol protocol)
{
    return DavManager::davProtocol(protocol)->useMultiget();
}

QString ProtocolInfo::principalHomeSet(KDAV::Protocol protocol)
{
    return DavManager::davProtocol(protocol)->principalHomeSet();
}

QString ProtocolInfo::principalHomeSetNS(KDAV::Protocol protocol)
{
    return DavManager::davProtocol(protocol)->principalHomeSetNS();
}

QLatin1String ProtocolInfo::protocolName(KDAV::Protocol protocol)
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

KDAV::Protocol ProtocolInfo::protocolByName(const QString &name)
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

QString ProtocolInfo::contactsMimeType(KDAV::Protocol protocol)
{
    QString ret;

    if (protocol == KDAV::CardDav) {
        ret = QStringLiteral("text/vcard");
    } else if (protocol == KDAV::GroupDav) {
        ret = QStringLiteral("text/x-vcard");
    }

    return ret;
}
