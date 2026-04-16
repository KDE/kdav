// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include "enums.h"
#include <QLatin1StringView>
#include <optional>

class QXmlStreamWriter;

namespace KDAV
{

class DavCollection;

namespace Xml
{

static constexpr QLatin1StringView davNS = QLatin1StringView("DAV:");
static constexpr QLatin1StringView caldavNS = QLatin1StringView("urn:ietf:params:xml:ns:caldav");
static constexpr QLatin1StringView carddavNS = QLatin1StringView("urn:ietf:params:xml:ns:carddav");
static constexpr QLatin1StringView icalNS = QLatin1StringView("http://apple.com/ns/ical/");

void writeMkCol(QXmlStreamWriter &writer, DavCollection &collection);
void writeCollectionProperties(QXmlStreamWriter &writer, DavCollection &collection, std::optional<Protocol> protocol);

} // end namespace Xml
} // end namespace KDAV
