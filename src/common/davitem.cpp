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

#include "davitem.h"

#include "davurl.h"

using namespace KDAV;

class DavItemPrivate : public QSharedData
{
public:
    DavUrl mUrl;
    QString mContentType;
    QByteArray mData;
    QString mEtag;
};

DavItem::DavItem()
    : d(new DavItemPrivate)
{
}

DavItem::DavItem(const DavUrl &url, const QString &contentType, const QByteArray &data, const QString &etag)
    : d(new DavItemPrivate)
{
    d->mUrl = url;
    d->mContentType = contentType;
    d->mData = data;
    d->mEtag = etag;
}

DavItem::DavItem(const DavItem &other) = default;
DavItem &DavItem::operator=(const DavItem &other) = default;
DavItem::~DavItem() = default;

void DavItem::setUrl(const DavUrl &url)
{
    d->mUrl = url;
}

DavUrl DavItem::url() const
{
    return d->mUrl;
}

void DavItem::setContentType(const QString &contentType)
{
    d->mContentType = contentType;
}

QString DavItem::contentType() const
{
    return d->mContentType;
}

void DavItem::setData(const QByteArray &data)
{
    d->mData = data;
}

QByteArray DavItem::data() const
{
    return d->mData;
}

void DavItem::setEtag(const QString &etag)
{
    d->mEtag = etag;
}

QString DavItem::etag() const
{
    return d->mEtag;
}

QDataStream &KDAV::operator<<(QDataStream &stream, const DavItem &item)
{
    stream << item.url();
    stream << item.contentType();
    stream << item.data();
    stream << item.etag();

    return stream;
}

QDataStream &KDAV::operator>>(QDataStream &stream, DavItem &item)
{
    QString contentType, etag;
    DavUrl url;
    QByteArray data;

    stream >> url;
    stream >> contentType;
    stream >> data;
    stream >> etag;

    item = DavItem(url, contentType, data, etag);

    return stream;
}
