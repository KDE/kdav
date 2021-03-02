/*
    SPDX-FileCopyrightText: 2019 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_PROTOCOLINFO_H
#define KDAV_PROTOCOLINFO_H

#include "enums.h"
#include "kdav_export.h"

namespace KDAV
{
/** Information about a DAV protocol. */
namespace ProtocolInfo
{
/**
 * Returns whether the @p protocol dialect supports the MULTIGET command.
 *
 * If MULTIGET is supported, the content of all DAV resources
 * can be fetched in Akonadi::ResourceBase::retrieveItems() already and
 * there is no need to call Akonadi::ResourceBase::retrieveItem() for every single
 * DAV resource.
 */
KDAV_EXPORT Q_REQUIRED_RESULT bool useMultiget(KDAV::Protocol protocol);

/** Returns the principal home set of @p protocol. */
KDAV_EXPORT Q_REQUIRED_RESULT QString principalHomeSet(KDAV::Protocol protocol);

/** Returns the principal home set namespace of @p protocol. */
KDAV_EXPORT Q_REQUIRED_RESULT QString principalHomeSetNS(KDAV::Protocol protocol);

/**
 * Returns the untranslated name of the given DAV @p protocol dialect.
 */
KDAV_EXPORT Q_REQUIRED_RESULT QString protocolName(KDAV::Protocol protocol);

/**
 * Returns the protocol matching the given name. This is the opposite of
 * ProtocolInfo::protocolName().
 */
KDAV_EXPORT Q_REQUIRED_RESULT KDAV::Protocol protocolByName(const QString &name);

/**
 * Returns the mimetype that shall be used for contact DAV resources using @p protocol.
 */
KDAV_EXPORT Q_REQUIRED_RESULT QString contactsMimeType(KDAV::Protocol protocol);
}

}

#endif // KDAV_PROTOCOLINFO_H
