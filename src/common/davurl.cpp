/*
    Copyright (c) 2010 Tobias Koenig <tokoe@kde.org>

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

#include "davurl.h"

#include "enums.h"

using namespace KDAV;

DavUrl::DavUrl()
    : mProtocol(KDAV::CalDav)
{
}

DavUrl::DavUrl(const QUrl &url, Protocol protocol)
    : mUrl(url), mProtocol(protocol)
{
}

void DavUrl::setUrl(const QUrl &url)
{
    mUrl = url;
}

QUrl DavUrl::url() const
{
    return mUrl;
}

void DavUrl::setProtocol(Protocol protocol)
{
    mProtocol = protocol;
}

Protocol DavUrl::protocol() const
{
    return mProtocol;
}

QString DavUrl::toDisplayString() const
{
    auto url = mUrl;
    url.setUserInfo(QString());
    return url.toDisplayString();
}

QDataStream &KDAV::operator<<(QDataStream &stream, const DavUrl &url)
{
    stream << QString::number(url.protocol());
    stream << url.url();

    return stream;
}

QDataStream &KDAV::operator>>(QDataStream &stream, DavUrl &davUrl)
{
    QUrl url;
    QString p;

    stream >> p;
    stream >> url;

    davUrl = DavUrl(url, static_cast<Protocol>( p.toInt() ));

    return stream;
}
