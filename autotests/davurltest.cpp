/*
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

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

#include "davurltest.h"

#include <KDAV/DavUrl>

#include <QDataStream>
#include <QTest>

void DavUrlTest::createEmpty()
{
    KDAV::DavUrl davUrl;

    QCOMPARE(davUrl.protocol(), KDAV::CalDav);
    QCOMPARE(davUrl.url(), QUrl());
}

void DavUrlTest::storeTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    QCOMPARE(davUrl.protocol(), KDAV::CardDav);
    QCOMPARE(davUrl.url(), url);
    QCOMPARE(davUrl.toDisplayString(), QStringLiteral("test://test"));
}

void DavUrlTest::setTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl;

    davUrl.setProtocol(KDAV::CardDav);
    davUrl.setUrl(url);

    QCOMPARE(davUrl.protocol(), KDAV::CardDav);
    QCOMPARE(davUrl.url(), url);
    QCOMPARE(davUrl.toDisplayString(), QStringLiteral("test://test"));
}

void DavUrlTest::serializeTest()
{
    KDAV::DavUrl davUrl1, davUrl2;

    QUrl url(QStringLiteral("test://me:pw@test"));
    davUrl1.setProtocol(KDAV::CardDav);
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
