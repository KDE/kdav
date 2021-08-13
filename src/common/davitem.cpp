/*
    SPDX-FileCopyrightText: 2009 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
DavItem::DavItem(DavItem &&) = default;
DavItem &DavItem::operator=(const DavItem &other) = default;
DavItem &DavItem::operator=(DavItem &&) = default;
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
    QString contentType;
    QString etag;
    DavUrl url;
    QByteArray data;

    stream >> url;
    stream >> contentType;
    stream >> data;
    stream >> etag;

    item = DavItem(url, contentType, data, etag);

    return stream;
}
