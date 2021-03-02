/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVMULTIGETPROTOCOL_H
#define KDAV_DAVMULTIGETPROTOCOL_H

#include "kdav_export.h"

#include "davprotocolbase_p.h"

namespace KDAV
{
/**
 * @short Base class for protocols that implement MULTIGET capabilities
 */
class DavMultigetProtocol : public DavProtocolBase
{
public:
    /**
     * Destroys the DAV protocol
     */
    virtual ~DavMultigetProtocol();

    /**
     * Returns the XML document that represents a MULTIGET DAV query to
     * list all DAV resources with the given @p urls.
     */
    virtual XMLQueryBuilder::Ptr itemsReportQuery(const QStringList &urls) const = 0;

    /**
     * Returns the namespace used by protocol-specific elements found in responses.
     */
    virtual QString responseNamespace() const = 0;

    /**
     * Returns the tag name of data elements found in responses.
     */
    virtual QString dataTagName() const = 0;
};
}

#endif
