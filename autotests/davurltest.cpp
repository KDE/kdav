/*
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

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

#include "davurltest.h"

#include <KDAV2/DavUrl>

#include <QDataStream>
#include <QTest>

void DavUrlTest::createEmpty()
{
    KDAV2::DavUrl davUrl;

    QCOMPARE(davUrl.protocol(), KDAV2::CalDav);
    QCOMPARE(davUrl.url(), QUrl());
}

void DavUrlTest::storeTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);

    QCOMPARE(davUrl.protocol(), KDAV2::CardDav);
    QCOMPARE(davUrl.url(), url);
    QCOMPARE(davUrl.toDisplayString(), QStringLiteral("test://test"));
}

void DavUrlTest::setTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl;

    davUrl.setProtocol(KDAV2::CardDav);
    davUrl.setUrl(url);

    QCOMPARE(davUrl.protocol(), KDAV2::CardDav);
    QCOMPARE(davUrl.url(), url);
    QCOMPARE(davUrl.toDisplayString(), QStringLiteral("test://test"));
}

void DavUrlTest::serializeTest()
{
    KDAV2::DavUrl davUrl1, davUrl2;

    QUrl url(QStringLiteral("test://me:pw@test"));
    davUrl1.setProtocol(KDAV2::CardDav);
    davUrl1.setUrl(url);

    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    s << davUrl1;

    QDataStream t(&data, QIODevice::ReadOnly);
    t >> davUrl2;

    QCOMPARE(davUrl2.protocol(), davUrl1.protocol());
    QCOMPARE(davUrl2.url(), davUrl1.url());
}

QTEST_MAIN(DavUrlTest)
