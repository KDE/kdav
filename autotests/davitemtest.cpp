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

#include "davitemtest.h"

#include <KDAV2/DavItem>
#include <KDAV2/DavUrl>

#include <QDataStream>
#include <QTest>

void DavItemTest::createEmpty()
{
    KDAV2::DavItem davItem;

    QCOMPARE(davItem.url().toDisplayString(), QString());
    QCOMPARE(davItem.contentType(), QString());
    QCOMPARE(davItem.data(), QByteArray());
    QCOMPARE(davItem.etag(), QString());
}


void DavItemTest::storeTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);
    KDAV2::DavItem davItem(davUrl, QStringLiteral("text/test"), QByteArray("data"), QStringLiteral("991233434-234345"));

    QCOMPARE(davItem.url().protocol(), KDAV2::CardDav);
    QCOMPARE(davItem.contentType(), QStringLiteral("text/test"));
    QCOMPARE(davItem.data(), QByteArray("data"));
    QCOMPARE(davItem.etag(), QStringLiteral("991233434-234345"));
}

void DavItemTest::setTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);
    KDAV2::DavItem davItem;

    davItem.setUrl(davUrl);
    davItem.setContentType(QStringLiteral("text/test"));
    davItem.setData(QByteArray("data"));
    davItem.setEtag(QStringLiteral("991233434-234345"));

    QCOMPARE(davItem.url().protocol(), KDAV2::CardDav);
    QCOMPARE(davItem.contentType(), QStringLiteral("text/test"));
    QCOMPARE(davItem.data(), QByteArray("data"));
    QCOMPARE(davItem.etag(), QStringLiteral("991233434-234345"));
}

void DavItemTest::copyTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);
    KDAV2::DavItem davItem(davUrl, QStringLiteral("text/test"), QByteArray("data"), QStringLiteral("991233434-234345"));

    KDAV2::DavItem davItemCopy1(davItem);
    QCOMPARE(davItemCopy1.url().protocol(), davItem.url().protocol());
    QCOMPARE(davItemCopy1.url().url(), davItem.url().url());
    QCOMPARE(davItemCopy1.contentType(), davItem.contentType());
    QCOMPARE(davItemCopy1.data(), davItem.data());
    QCOMPARE(davItemCopy1.etag(), davItem.etag());
 
    KDAV2::DavItem davItemCopy2;
    davItemCopy2 = davItem;
    QCOMPARE(davItemCopy2.url().protocol(), davItem.url().protocol());
    QCOMPARE(davItemCopy2.url().url(), davItem.url().url());
    QCOMPARE(davItemCopy2.contentType(), davItem.contentType());
    QCOMPARE(davItemCopy2.data(), davItem.data());
    QCOMPARE(davItemCopy2.etag(), davItem.etag());
}

void DavItemTest::serializeTest()
{
    KDAV2::DavItem davItem1, davItem2;

    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);

    davItem1.setUrl(davUrl);
    davItem1.setContentType(QStringLiteral("text/test"));
    davItem1.setData(QByteArray("data"));
    davItem1.setEtag(QStringLiteral("991233434-234345"));

    QByteArray data;
    QDataStream s(&data, QIODevice::WriteOnly);
    s << davItem1;

    QDataStream t(&data, QIODevice::ReadOnly);
    t >> davItem2;

    QCOMPARE(davItem2.url().protocol(), davItem1.url().protocol());
    QCOMPARE(davItem2.url().url(), davItem1.url().url());
    QCOMPARE(davItem2.contentType(), davItem1.contentType());
    QCOMPARE(davItem2.data(), davItem1.data());
    QCOMPARE(davItem2.etag(), davItem1.etag());
}

QTEST_MAIN(DavItemTest)
