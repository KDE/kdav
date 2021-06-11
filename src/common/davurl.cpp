/*
    SPDX-FileCopyrightText: 2010 Tobias Koenig <tokoe@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davurl.h"


using namespace KDAV;

namespace KDAV
{
class DavUrlPrivate : public QSharedData
{
public:
    Protocol mProtocol = KDAV::CalDav;
    QUrl mUrl;
};
}

DavUrl::DavUrl()
    : d(new DavUrlPrivate)
{
}

DavUrl::DavUrl(const QUrl &url, Protocol protocol)
    : d(new DavUrlPrivate)
{
    d->mUrl = url;
    d->mProtocol = protocol;
}

DavUrl::DavUrl(const DavUrl &) = default;
DavUrl::DavUrl(DavUrl &&) = default;
DavUrl::~DavUrl() = default;
DavUrl &DavUrl::operator=(const DavUrl &) = default;
DavUrl &DavUrl::operator=(DavUrl &&) = default;

void DavUrl::setUrl(const QUrl &url)
{
    d->mUrl = url;
}

QUrl DavUrl::url() const
{
    return d->mUrl;
}

void DavUrl::setProtocol(Protocol protocol)
{
    d->mProtocol = protocol;
}

Protocol DavUrl::protocol() const
{
    return d->mProtocol;
}

QString DavUrl::toDisplayString() const
{
    auto url = d->mUrl;
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

    davUrl = DavUrl(url, static_cast<Protocol>(p.toInt()));

    return stream;
}
