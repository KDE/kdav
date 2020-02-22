/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
