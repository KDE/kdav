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

#ifndef GROUPDAVPROTOCOL_H
#define GROUPDAVPROTOCOL_H

#include "common/davprotocolbase.h"

class GroupdavProtocol : public KDAV::DavProtocolBase
{
public:
    GroupdavProtocol();
    bool supportsPrincipals() const override;
    bool useReport() const override;
    bool useMultiget() const override;
    KDAV::XMLQueryBuilder::Ptr collectionsQuery() const override;
    QString collectionsXQuery() const override;
    QVector<KDAV::XMLQueryBuilder::Ptr> itemsQueries() const override;

    KDAV::DavCollection::ContentTypes collectionContentTypes(const QDomElement &propstat) const override;
};

#endif
