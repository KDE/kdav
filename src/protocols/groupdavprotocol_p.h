/*
    Copyright (c) 2009 Grégory Oestreicher <greg@kamago.net>

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

#ifndef GROUPDAVPROTOCOL_H
#define GROUPDAVPROTOCOL_H

#include "common/davprotocolbase_p.h"

class GroupdavProtocol : public KDAV::DavProtocolBase
{
public:
    GroupdavProtocol();
    Q_REQUIRED_RESULT bool supportsPrincipals() const override;
    Q_REQUIRED_RESULT bool useReport() const override;
    Q_REQUIRED_RESULT bool useMultiget() const override;
    Q_REQUIRED_RESULT KDAV::XMLQueryBuilder::Ptr collectionsQuery() const override;
    Q_REQUIRED_RESULT QString collectionsXQuery() const override;
    Q_REQUIRED_RESULT QVector<KDAV::XMLQueryBuilder::Ptr> itemsQueries() const override;

    Q_REQUIRED_RESULT KDAV::DavCollection::ContentTypes collectionContentTypes(const QDomElement &propstat) const override;
};

#endif
