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

QString ProtocolInfo::protocolName(KDAV::Protocol protocol)
{
    switch (protocol) {
    case KDAV::CalDav:
        return QStringLiteral("CalDav");
    case KDAV::CardDav:
        return QStringLiteral("CardDav");
    case KDAV::GroupDav:
        return QStringLiteral("GroupDav");
    }
    return {};
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
