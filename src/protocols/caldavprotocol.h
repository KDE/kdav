/*
    Copyright (c) 2009 Grégory Oestreicher <greg@kamago.net>

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

#ifndef KDAV_CALDAVPROTOCOL_H
#define KDAV_CALDAVPROTOCOL_H

#include "common/davmultigetprotocol.h"

class CaldavProtocol : public KDAV::DavMultigetProtocol
{
public:
    CaldavProtocol();
    bool supportsPrincipals() const override;
    bool useReport() const override;
    bool useMultiget() const override;
    QString principalHomeSet() const override;
    QString principalHomeSetNS() const override;
    KDAV::XMLQueryBuilder::Ptr collectionsQuery() const override;
    QString collectionsXQuery() const override;
    QVector<KDAV::XMLQueryBuilder::Ptr> itemsQueries() const override;
    KDAV::XMLQueryBuilder::Ptr itemsReportQuery(const QStringList &urls) const override;
    QString responseNamespace() const override;
    QString dataTagName() const override;

    KDAV::DavCollection::ContentTypes collectionContentTypes(const QDomElement &propstat) const override;
};

#endif
