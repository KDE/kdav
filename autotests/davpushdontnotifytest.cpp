/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davpushdontnotifytest.h"

#include <KDAV/DavPushDontNotify>

#include <QTest>

using namespace Qt::Literals;

void DavPushDontNotifyTest::createEmpty()
{
    const auto davPushDontNotify = KDAV::DavPushDontNotify();

    QVERIFY(!davPushDontNotify.isIgnoreAll());
    QVERIFY(davPushDontNotify.ignoredUrls().isEmpty());
    QVERIFY(davPushDontNotify.isNull());
    QCOMPARE(davPushDontNotify.davHeaderValue(), QString());
}

void DavPushDontNotifyTest::ignoreAll()
{
    const auto davPushDontNotify = KDAV::DavPushDontNotify::ignoreAll();

    QVERIFY(davPushDontNotify.isIgnoreAll());
    QVERIFY(davPushDontNotify.ignoredUrls().isEmpty());
    QVERIFY(!davPushDontNotify.isNull());
    QCOMPARE(davPushDontNotify.davHeaderName(), QStringLiteral("DavPush-DontNotify"));
    QCOMPARE(davPushDontNotify.davHeaderValue(), QStringLiteral("*"));
}

void DavPushDontNotifyTest::ignoreUrls()
{
    auto url1 = u"https://example.com/webdav/subscriptions/TOKEN1"_s;
    auto url2 = u"https://example.com/webdav/subscriptions/TOKEN2"_s;
    const auto davPushDontNotify = KDAV::DavPushDontNotify::ignoreUrls({url1, url2});

    QVERIFY(!davPushDontNotify.isIgnoreAll());
    QVERIFY(!davPushDontNotify.ignoredUrls().isEmpty());
    QVERIFY(!davPushDontNotify.isNull());
    QCOMPARE(davPushDontNotify.davHeaderName(), QStringLiteral("DavPush-DontNotify"));
    QCOMPARE(davPushDontNotify.davHeaderValue(), QStringLiteral("\"%1\", \"%2\"").arg(url1, url2));
}

QTEST_MAIN(DavPushDontNotifyTest)

#include "moc_davpushdontnotifytest.cpp"
