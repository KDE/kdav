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

#include "davcollection.h"

#include <QColor>

using namespace KDAV;

class DavCollectionPrivate
{
public:
    DavCollectionPrivate(DavCollection *qPtr) : q(qPtr) {}

    void fillFrom(const DavCollectionPrivate &other);

    DavCollection *q;
    DavUtils::Protocol mProtocol;
    QString mCTag;
    QString mUrl;
    QString mDisplayName;
    QColor mColor;
    DavCollection::ContentTypes mContentTypes;
    DavUtils::Privileges mPrivileges;
};

void DavCollectionPrivate::fillFrom(const DavCollectionPrivate &other)
{
    mProtocol = other.mProtocol;
    mCTag = other.mCTag;
    mUrl = other.mUrl;
    mDisplayName = other.mDisplayName;
    mColor = other.mColor;
    mContentTypes = other.mContentTypes;
    mPrivileges = other.mPrivileges;
}


DavCollection::DavCollection()
    : d(std::unique_ptr<DavCollectionPrivate>(new DavCollectionPrivate(this)))
{
}

DavCollection::DavCollection(DavUtils::Protocol protocol, const QString &url, const QString &displayName, ContentTypes contentTypes)
    : d(std::unique_ptr<DavCollectionPrivate>(new DavCollectionPrivate(this)))
{
    d->mProtocol = protocol;
    d->mUrl = url;
    d->mDisplayName = displayName;
    d->mContentTypes = contentTypes;
    d->mPrivileges = DavUtils::All;
}

DavCollection::DavCollection(const DavCollection &other)
    : d(std::unique_ptr<DavCollectionPrivate>(new DavCollectionPrivate(this)))
{
    d->fillFrom(*other.d.get());
}

DavCollection &DavCollection::operator=(const DavCollection &other)
{
    d->fillFrom(*other.d.get());
    return *this;
}

DavCollection::~DavCollection()
{
}

void DavCollection::setProtocol(DavUtils::Protocol protocol)
{
    d->mProtocol = protocol;
}

DavUtils::Protocol DavCollection::protocol() const
{
    return d->mProtocol;
}

void DavCollection::setCTag(const QString &ctag)
{
    d->mCTag = ctag;
}

QString DavCollection::CTag() const
{
    return d->mCTag;
}

void DavCollection::setUrl(const QString &url)
{
    d->mUrl = url;
}

QString DavCollection::url() const
{
    return d->mUrl;
}

void DavCollection::setDisplayName(const QString &displayName)
{
    d->mDisplayName = displayName;
}

QString DavCollection::displayName() const
{
    return d->mDisplayName;
}

void DavCollection::setColor(const QColor &color)
{
    d->mColor = color;
}

QColor DavCollection::color() const
{
    return d->mColor;
}

void DavCollection::setContentTypes(ContentTypes contentTypes)
{
    d->mContentTypes = contentTypes;
}

DavCollection::ContentTypes DavCollection::contentTypes() const
{
    return d->mContentTypes;
}

void DavCollection::setPrivileges(DavUtils::Privileges privs)
{
    d->mPrivileges = privs;
}

DavUtils::Privileges DavCollection::privileges() const
{
    return d->mPrivileges;
}

