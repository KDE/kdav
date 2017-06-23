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

#include "davcollectiontest.h"

#include <KDAV/DavCollection>
#include <KDAV/DavUrl>

// #include <QColor>
#include <QDebug>
#include <QDataStream>
#include <QTest>

void DavCollectionTest::createEmpty()
{
    KDAV::DavCollection davCollection;

    QCOMPARE(davCollection.url().protocol(), KDAV::CalDav);
    QCOMPARE(davCollection.CTag(), QString());
    QCOMPARE(davCollection.displayName(), QString());
    // QCOMPARE(davCollection.color(), QColor());
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::ContentTypes());
    QCOMPARE(davCollection.privileges(), KDAV::Privileges());
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
    // QCOMPARE(davCollection.color(), QColor());
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    QCOMPARE(davCollection.privileges(), KDAV::All);
}

void DavCollectionTest::setTest()
{
    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavCollection davCollection;

    davCollection.setUrl(davUrl);
    davCollection.setCTag(QStringLiteral("ctag"));
    davCollection.setDisplayName(QStringLiteral("myname"));
    // davCollection.setColor(QColor(1,2,3));
    davCollection.setContentTypes(KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    davCollection.setPrivileges(KDAV::Read | KDAV::Write);

    QCOMPARE(davCollection.url().protocol(), KDAV::CardDav);
    QCOMPARE(davCollection.url().url(), url);
    QCOMPARE(davCollection.CTag(), QStringLiteral("ctag"));
    QCOMPARE(davCollection.displayName(), QStringLiteral("myname"));
    // QCOMPARE(davCollection.color(), QColor(1,2,3));
    QCOMPARE(davCollection.contentTypes(), KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    QCOMPARE(davCollection.privileges(), KDAV::Read | KDAV::Write);
}

void DavCollectionTest::copyTest()
{
    KDAV::DavCollection davCollection;

    QUrl url(QStringLiteral("test://me:pw@test"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    davCollection.setUrl(davUrl);
    davCollection.setCTag(QStringLiteral("ctag"));
    davCollection.setDisplayName(QStringLiteral("myname"));
    // davCollection.setColor(QColor(1,2,3));
    davCollection.setContentTypes(KDAV::DavCollection::Events | KDAV::DavCollection::Todos);
    davCollection.setPrivileges(KDAV::Read | KDAV::Write);

    KDAV::DavCollection copy1(davCollection);
    QCOMPARE(copy1.url().protocol(), davCollection.url().protocol());
    QCOMPARE(copy1.url().url(), davCollection.url().url());
    QCOMPARE(copy1.CTag(), davCollection.CTag());
    QCOMPARE(copy1.displayName(), davCollection.displayName());
    QCOMPARE(copy1.color(), davCollection.color());
    QCOMPARE(copy1.contentTypes(), davCollection.contentTypes());
    QCOMPARE(copy1.privileges(), davCollection.privileges());

    KDAV::DavCollection copy2;
    copy2 = davCollection;

    QCOMPARE(copy2.url().protocol(), davCollection.url().protocol());
    QCOMPARE(copy2.url().url(), davCollection.url().url());
    QCOMPARE(copy2.CTag(), davCollection.CTag());
    QCOMPARE(copy2.displayName(), davCollection.displayName());
    QCOMPARE(copy2.color(), davCollection.color());
    QCOMPARE(copy2.contentTypes(), davCollection.contentTypes());
    QCOMPARE(copy2.privileges(), davCollection.privileges());
}

QTEST_GUILESS_MAIN(DavCollectionTest)
