// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davitemmodifyjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavItem>
#include <KDAV/DavItemModifyJob>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavItemModifyJobTest::initTestCase()
{
    // To let ctest exit, we shouldn't start kio_http_cache_cleaner
    qputenv("KIO_DISABLE_CACHE_CLEANER", "yes");
}

void DavItemModifyJobTest::runSuccessfulTest()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-put-success.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-get-refreshed.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, u"text/x-vcard"_s, QByteArray("BEGIN:VCARD\r\nEND:VCARD\r\n"), u"\"original-etag\""_s);

    auto job = new KDAV::DavItemModifyJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->item().etag(), u"newetag123"_s);
}

void DavItemModifyJobTest::runSuccessWithLocationRedirect()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-put-redirect.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-get-redirected.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, u"text/x-vcard"_s, QByteArray("BEGIN:VCARD\r\nEND:VCARD\r\n"), u"\"original-etag\""_s);

    auto job = new KDAV::DavItemModifyJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->item().url().url().path(), u"/new-item"_s);
    QCOMPARE(job->item().etag(), u"new-location-etag"_s);
}

void DavItemModifyJobTest::runServerErrorTest()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-put-error.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, u"text/x-vcard"_s, QByteArray("BEGIN:VCARD\r\nEND:VCARD\r\n"), u"\"original-etag\""_s);

    auto job = new KDAV::DavItemModifyJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QVERIFY(job->canRetryLater());
}

void DavItemModifyJobTest::runConflictTest()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-put-conflict.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmodifyjob-get-conflict-item.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, u"text/x-vcard"_s, QByteArray("BEGIN:VCARD\r\nEND:VCARD\r\n"), u"\"original-etag\""_s);

    auto job = new KDAV::DavItemModifyJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QVERIFY(job->hasConflict());
    QCOMPARE(job->freshItem().etag(), u"server-etag-456"_s);
    QCOMPARE(job->freshResponseCode(), 200);
}

QTEST_MAIN(DavItemModifyJobTest)

#include "moc_davitemmodifyjobtest.cpp"
