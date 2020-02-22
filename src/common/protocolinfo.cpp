/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
