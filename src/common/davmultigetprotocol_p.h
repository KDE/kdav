/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVMULTIGETPROTOCOL_H
#define KDAV_DAVMULTIGETPROTOCOL_H

#include "kdav_export.h"

#include "davprotocolbase_p.h"

class QXmlStreamWriter;

namespace KDAV
{

class DavCollection;

/*!
 * \internal
 * \brief Base class for protocols that implement MULTIGET capabilities
 */
class DavMultigetProtocol : public DavProtocolBase
{
public:
    /*!
     * Destroys the DAV protocol
     */
    ~DavMultigetProtocol() override;

    /*!
     * Returns the XML document that represents a MULTIGET DAV query to
     * list all DAV resources with the given \a urls.
     */
    virtual XMLQueryBuilder::Ptr itemsReportQuery(const QStringList &urls) const = 0;

    /*!
     * Returns the namespace used by protocol-specific elements found in responses.
     */
    virtual QString responseNamespace() const = 0;

    /*!
     * Returns the tag name of data elements found in responses.
     */
    virtual QString dataTagName() const = 0;

    virtual void writeMkCol(QXmlStreamWriter &writer, DavCollection &collection) const = 0;
};

namespace Xml
{

using namespace Qt::StringLiterals;

static constexpr QLatin1StringView davNS = QLatin1StringView("DAV:");
static constexpr QLatin1StringView caldavNS = QLatin1StringView("urn:ietf:params:xml:ns:caldav");
static constexpr QLatin1StringView carddavNS = QLatin1StringView("urn:ietf:params:xml:ns:carddav");
static constexpr QLatin1StringView icalNS = QLatin1StringView("http://apple.com/ns/ical/");
} // end namespace XML

} // end namespace KDAV

#endif
