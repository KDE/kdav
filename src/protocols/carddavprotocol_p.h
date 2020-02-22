/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_CARDDAVPROTOCOL_H
#define KDAV_CARDDAVPROTOCOL_H

#include "common/davmultigetprotocol_p.h"

class CarddavProtocol : public KDAV::DavMultigetProtocol
{
public:
    CarddavProtocol();
    Q_REQUIRED_RESULT bool supportsPrincipals() const override;
    Q_REQUIRED_RESULT bool useReport() const override;
    Q_REQUIRED_RESULT bool useMultiget() const override;
    Q_REQUIRED_RESULT QString principalHomeSet() const override;
    Q_REQUIRED_RESULT QString principalHomeSetNS() const override;
    Q_REQUIRED_RESULT KDAV::XMLQueryBuilder::Ptr collectionsQuery() const override;
    Q_REQUIRED_RESULT QString collectionsXQuery() const override;
    Q_REQUIRED_RESULT QVector<KDAV::XMLQueryBuilder::Ptr> itemsQueries() const override;
    Q_REQUIRED_RESULT KDAV::XMLQueryBuilder::Ptr itemsReportQuery(const QStringList &urls) const override;
    Q_REQUIRED_RESULT QString responseNamespace() const override;
    Q_REQUIRED_RESULT QString dataTagName() const override;

    Q_REQUIRED_RESULT KDAV::DavCollection::ContentTypes collectionContentTypes(const QDomElement &propstat) const override;
};

#endif
