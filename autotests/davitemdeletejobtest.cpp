// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davitemdeletejobtest.h"
#include "fakeserver.h"

#include <KDAV/DavError>
#include <KDAV/DavItem>
#include <KDAV/DavItemDeleteJob>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavItemDeleteJobTest::deleteSucceeds()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /item HTTP/1.1",
        "C: If-Match: \"test-etag\"",
        "S: HTTP/1.1 200 OK",
        "S: Date: Mon, 21 Apr 2026 15:00:00 GMT",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto job = new KDAV::DavItemDeleteJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(!job->hasConflict());
}

void DavItemDeleteJobTest::deleteServerError()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /item HTTP/1.1",
        "C: If-Match: \"test-etag\"",
        "S: HTTP/1.1 403 Forbidden",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto job = new KDAV::DavItemDeleteJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_ITEMDELETE));
    QCOMPARE(job->latestResponseCode(), 403);
    QVERIFY(!job->hasConflict());
}

void DavItemDeleteJobTest::deleteIgnores404()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /item HTTP/1.1",
        "C: If-Match: \"test-etag\"",
        "S: HTTP/1.1 404 Not Found",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto job = new KDAV::DavItemDeleteJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(!job->hasConflict());
}

void DavItemDeleteJobTest::deleteIgnores410()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /item HTTP/1.1",
        "C: If-Match: \"test-etag\"",
        "S: HTTP/1.1 410 Gone",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto job = new KDAV::DavItemDeleteJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(!job->hasConflict());
}

void DavItemDeleteJobTest::deleteConflict()
{
    FakeServer fakeServer;
    // DELETE returns 412 Precondition Failed (If-Match mismatch)
    fakeServer.addScenario({
        "C: DELETE /item HTTP/1.1",
        "C: If-Match: \"test-etag\"",
        "S: HTTP/1.1 412 Precondition Failed",
        "X",
    });
    // DavItemDeleteJob then fetches the fresh item via GET
    fakeServer.addScenario({
        "C: GET /item HTTP/1.1",
        "C: User-Agent: KDE DAV groupware client",
        "S: HTTP/1.0 200 OK",
        "S: Date: Mon, 21 Apr 2026 15:00:00 GMT",
        "S: ETag: \"new-etag\"",
        "S: Content-Type: text/x-vcard; charset=utf-8",
        "D: BEGIN:VCARD",
        "D: VERSION:3.0",
        "D: UID:12345678-1234-1234-1234-123456789abc",
        "D: FN:John Doe",
        "D: END:VCARD",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/item"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::DavItem item(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto job = new KDAV::DavItemDeleteJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_ITEMDELETE));
    QVERIFY(job->hasConflict());
    QCOMPARE(job->latestResponseCode(), 412);

    const KDAV::DavItem freshItem = job->freshItem();
    QVERIFY(!freshItem.data().isEmpty());
}

QTEST_MAIN(DavItemDeleteJobTest)

#include "moc_davitemdeletejobtest.cpp"
