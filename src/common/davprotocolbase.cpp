/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davprotocolbase_p.h"

#include <QVariant>

using namespace KDAV;

XMLQueryBuilder::~XMLQueryBuilder()
{
}

void XMLQueryBuilder::setParameter(const QString &key, const QVariant &value)
{
    mParameters[key] = value;
}

QVariant XMLQueryBuilder::parameter(const QString &key) const
{
    QVariant ret;
    if (mParameters.contains(key)) {
        ret = mParameters.value(key);
    }
    return ret;
}

DavProtocolBase::~DavProtocolBase()
{
}

QString DavProtocolBase::principalHomeSet() const
{
    return QString();
}

QString DavProtocolBase::principalHomeSetNS() const
{
    return QString();
}
