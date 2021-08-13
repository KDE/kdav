/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
    KDAV::DavUrl davUrl1;
    KDAV::DavUrl davUrl2;

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
