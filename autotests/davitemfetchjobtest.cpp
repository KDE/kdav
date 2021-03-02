/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavItemFetchJob>

#include <QTest>

void DavItemFetchJobTest::initTestCase()
{
    // To avoid a runtime dependency on klauncher
    qputenv("KDE_FORK_SLAVES", "yes");
    // To let ctest exit, we shouldn't start kio_http_cache_cleaner
    qputenv("KIO_DISABLE_CACHE_CLEANER", "yes");
}

void DavItemFetchJobTest::runSuccessfullTest()
{
    FakeServer fakeServer(5989);
    QUrl url(QStringLiteral("http://localhost/item"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    KDAV::DavItem item(davUrl, QString(), QByteArray(), QString());

    auto job = new KDAV::DavItemFetchJob(item);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemfetchjob.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    QCOMPARE(item.data(), QByteArray());
    QCOMPARE(item.etag(), QString());
    QCOMPARE(item.contentType(), QString());

    item = job->item();
    QByteArray data(
        "BEGIN:VCARD\r\nVERSION:3.0\r\nPRODID:-//Kolab//iRony DAV Server 0.3.1//Sabre//Sabre VObject "
        "2.1.7//EN\r\nUID:12345678-1234-1234-1234-123456789abc\r\nFN:John2 "
        "Doe\r\nN:Doe;John2;;;\r\nEMAIL;TYPE=INTERNET;TYPE=HOME:john2.doe@example.com\r\nREV;VALUE=DATE-TIME:20170104T182647Z\r\nEND:VCARD\r\n");
    QCOMPARE(item.data(), data);
    QCOMPARE(item.etag(), QStringLiteral("7a33141f192d904d-47"));
    QCOMPARE(item.contentType(), QStringLiteral("text/x-vcard"));
}

QTEST_MAIN(DavItemFetchJobTest)
