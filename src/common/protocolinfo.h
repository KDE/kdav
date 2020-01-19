/*
    Copyright (c) 2019 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef KDAV_PROTOCOLINFO_H
#define KDAV_PROTOCOLINFO_H

#include "kdav_export.h"
#include "enums.h"

class QLatin1String;

namespace KDAV
{

/** Information about a DAV protocol. */
namespace ProtocolInfo
{
    /**
     * Returns whether the @p protocol dialect supports the MULTIGET command.
     *
     * If MULTIGET is supported, the content of all dav resources
     * can be fetched in ResourceBase::retrieveItems() already and
     * there is no need to call ResourceBase::retrieveItem() for every single
     * dav resource.
     */
    KDAV_EXPORT Q_REQUIRED_RESULT bool useMultiget(KDAV::Protocol protocol);

    /** Returns the principal home set of @p protocol. */
    KDAV_EXPORT Q_REQUIRED_RESULT QString principalHomeSet(KDAV::Protocol protocol);

    /** Returns the principal home set namespace of @p protocol. */
    KDAV_EXPORT Q_REQUIRED_RESULT QString principalHomeSetNS(KDAV::Protocol protocol);

    /**
     * Returns the untranslated name of the given DAV @p protocol dialect.
     */
    KDAV_EXPORT Q_REQUIRED_RESULT QLatin1String protocolName(KDAV::Protocol protocol);

    /**
     * Returns the protocol matching the given name. This is the opposite of
     * Utils::protocolName().
     */
    KDAV_EXPORT Q_REQUIRED_RESULT KDAV::Protocol protocolByName(const QString &name);

    /**
     * Returns the mimetype that shall be used for contact DAV resources using @p protocol.
     */
    KDAV_EXPORT Q_REQUIRED_RESULT QString contactsMimeType(KDAV::Protocol protocol);
}

}

#endif // KDAV_PROTOCOLINFO_H
