/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectiontest.h"

#include <KDAV/DavCollection>
#include <KDAV/DavPushSupport>
#include <KDAV/DavUrl>

#include <QColor>
#include <QTest>

void DavCollectionTest::createEmpty()
{
    KDAV::DavCollection davCollection;

    QCOMPARE(davCollection.url().protocol(), KDAV::CalDav);
    QCOMPARE(davCollection.CTag(), QString());
    QCOMPARE(davCollection.displayName(), QString());
    QCOMPARE(davCollection.color(), QColor());
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::ContentTypes());
    QCOMPARE(davCollection.privileges(), KDAV::Privileges());
    QCOMPARE(davCollection.davPushSupport(), KDAV::DavPushSupport());
}

void DavCollectionTest::storeTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavCollection davCollection(davUrl, QStringLiteral("myname"), KDAV::DavCollection::Events | KDAV::DavCollection::Todos);

    QCOMPARE(davCollection.url().protocol(), KDAV::CardDav);
    QCOMPARE(davCollection.url().url(), url);
    QCOMPARE(davCollection.CTag(), QString());
    QCOMPARE(davCollection.displayName(), QStringLiteral("myname"));
    QCOMPARE(davCollection.color(), QColor());
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    QCOMPARE(davCollection.privileges(), KDAV::All);
}

void DavCollectionTest::setTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    KDAV::DavPushSupport davPush{};
    davPush.setVapidPublicKey("TXlWYXBpZFB1YmxpY0tleQ");
    davPush.setTopic(QLatin1StringView("Topic"));

    KDAV::DavCollection davCollection;

    davCollection.setUrl(davUrl);
    davCollection.setCTag(QStringLiteral("ctag"));
    davCollection.setDisplayName(QStringLiteral("myname"));
    davCollection.setColor(QColor(1, 2, 3));
    davCollection.setContentTypes(KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    davCollection.setPrivileges(KDAV::Read | KDAV::Write);
    davCollection.setDavPushSupport(davPush);

    QCOMPARE(davCollection.url().protocol(), KDAV::CardDav);
    QCOMPARE(davCollection.url().url(), url);
    QCOMPARE(davCollection.CTag(), QStringLiteral("ctag"));
    QCOMPARE(davCollection.displayName(), QStringLiteral("myname"));
    QCOMPARE(davCollection.color(), QColor(1, 2, 3));
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    QCOMPARE(davCollection.privileges(), KDAV::Read | KDAV::Write);
    QCOMPARE(davCollection.davPushSupport(), davPush);
}

void DavCollectionTest::copyTest()
{
    KDAV::DavCollection davCollection;

    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    KDAV::DavPushSupport davPush{};
    davPush.setVapidPublicKey("TXlWYXBpZFB1YmxpY0tleQ");
    davPush.setTopic(QLatin1StringView("Topic"));

    davCollection.setUrl(davUrl);
    davCollection.setCTag(QStringLiteral("ctag"));
    davCollection.setDisplayName(QStringLiteral("myname"));
    davCollection.setColor(QColor(1, 2, 3));
    davCollection.setContentTypes(KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    davCollection.setPrivileges(KDAV::Read | KDAV::Write);
    davCollection.setDavPushSupport(davPush);

    KDAV::DavCollection copy1(davCollection);
    QCOMPARE(copy1.url().protocol(), davCollection.url().protocol());
    QCOMPARE(copy1.url().url(), davCollection.url().url());
    QCOMPARE(copy1.CTag(), davCollection.CTag());
    QCOMPARE(copy1.displayName(), davCollection.displayName());
    QCOMPARE(copy1.color(), davCollection.color());
    QCOMPARE(copy1.contentTypes(), davCollection.contentTypes());
    QCOMPARE(copy1.privileges(), davCollection.privileges());
    QCOMPARE(copy1.davPushSupport(), davPush);

    KDAV::DavCollection copy2;
    copy2 = davCollection;

    QCOMPARE(copy2.url().protocol(), davCollection.url().protocol());
    QCOMPARE(copy2.url().url(), davCollection.url().url());
    QCOMPARE(copy2.CTag(), davCollection.CTag());
    QCOMPARE(copy2.displayName(), davCollection.displayName());
    QCOMPARE(copy2.color(), davCollection.color());
    QCOMPARE(copy2.contentTypes(), davCollection.contentTypes());
    QCOMPARE(copy2.privileges(), davCollection.privileges());
    QCOMPARE(copy2.davPushSupport(), davPush);
}

QTEST_MAIN(DavCollectionTest)

#include "moc_davcollectiontest.cpp"
