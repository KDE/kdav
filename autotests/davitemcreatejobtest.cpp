// SPDX-FileCopyrightText: 2026 David Faure <faure@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davitemcreatejobtest.h"
#include "fakeserver.h"

#include <KDAV/DavItem>
#include <KDAV/DavItemCreateJob>
#include <KDAV/DavUrl>

#include <QRegularExpression>
#include <QTest>

using namespace Qt::StringLiterals;

using Scenarios = QList<QList<QByteArray>>;

static QList<QByteArray> putScenario(const QByteArray &path, const QList<QByteArray> &response)
{
    return QList<QByteArray>{"C: PUT " + path + " HTTP/1.1", "C: Content-Type: text/calendar", "C: If-None-Match: *"} + response + QList<QByteArray>{"X"};
}

static QList<QByteArray> getScenario(const QByteArray &path)
{
    return {
        "C: GET " + path + " HTTP/1.1",
        "S: HTTP/1.0 200 OK",
        "S: ETag: refreshed-etag",
        "S: Content-Type: text/calendar",
        "D: BEGIN:VCALENDAR",
        "D: END:VCALENDAR",
        "X",
    };
}

void DavItemCreateJobTest::createItem_data()
{
    QTest::addColumn<Scenarios>("scenarios");
    QTest::addColumn<QString>("expectedPath");
    QTest::addColumn<QString>("expectedWarning");

    QTest::newRow("no-location") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created"}), getScenario("/coll/item.ics")} << u"/coll/item.ics"_s
                                 << QString();

    // sabre/dav stores the item under its UID and says so with an absolute-path Location
    const QByteArray renamed = "/coll/uid%40example.com.ics";
    QTest::newRow("absolute-path") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: " + renamed}), getScenario(renamed)}
                                   << QString::fromLatin1(renamed) << QString();

    const QByteArray outside = "/calendars/dfaure%40kdab.com/coll/b89b3058-945c-4c1e-8cee-40b4d9612d37.ics";
    QTest::newRow("outside-collection") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: " + outside}),
                                                     getScenario(outside)}
                                        << QString::fromLatin1(outside) << u"stored outside its collection"_s;

    QTest::newRow("relative-path") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: renamed.ics"}),
                                                getScenario("/coll/renamed.ics")}
                                   << u"/coll/renamed.ics"_s << QString();

    // Dot segments are removed (RFC 3986 section 5.2.4) and cannot go above the root
    QTest::newRow("dot-segments") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: ../../../other/item.ics"}),
                                               getScenario("/other/item.ics")}
                                  << u"/other/item.ics"_s << u"stored outside its collection"_s;

    QTest::newRow("invalid") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: http://[bad/x"}),
                                          getScenario("/coll/item.ics")}
                             << u"/coll/item.ics"_s << u"Invalid Location header"_s;

    QTest::newRow("redirect") << Scenarios{putScenario("/coll/item.ics", {"S: HTTP/1.0 301 Moved Permanently", "S: Location: /other/item.ics"}),
                                           putScenario("/other/item.ics", {"S: HTTP/1.0 201 Created"}),
                                           getScenario("/other/item.ics")}
                              << u"/other/item.ics"_s << QString();
}

void DavItemCreateJobTest::createItem()
{
    QFETCH(Scenarios, scenarios);
    QFETCH(QString, expectedPath);
    QFETCH(QString, expectedWarning);

    FakeServer fakeServer;
    for (const auto &scenario : scenarios) {
        fakeServer.addScenario(scenario);
    }
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/coll/item.ics"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);
    KDAV::DavItem item(davUrl, u"text/calendar"_s, QByteArray("BEGIN:VCALENDAR\r\nEND:VCALENDAR\r\n"), QString());

    if (!expectedWarning.isEmpty()) {
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(expectedWarning));
    } else {
        QTest::failOnWarning(QRegularExpression(u"outside its collection"_s));
    }
    auto job = new KDAV::DavItemCreateJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QUrl expectedUrl(url);
    expectedUrl.setPath(expectedPath, QUrl::TolerantMode);
    QCOMPARE(job->item().url().url(), expectedUrl);
    QCOMPARE(job->item().etag(), u"refreshed-etag"_s);
}

void DavItemCreateJobTest::createItemWithDifferentEncoding()
{
    // The request has "@" and the Location "%40": same collection, no warning
    const QByteArray renamed = "/user%40example.com/renamed.ics";
    FakeServer fakeServer;
    fakeServer.addScenario(putScenario("/user@example.com/item.ics", {"S: HTTP/1.0 201 Created", "S: Location: " + renamed}));
    fakeServer.addScenario(getScenario(renamed));
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/user@example.com/item.ics"_s);
    url.setPort(fakeServer.port());
    KDAV::DavItem item(KDAV::DavUrl(url, KDAV::CalDav), u"text/calendar"_s, QByteArray("BEGIN:VCALENDAR\r\nEND:VCALENDAR\r\n"), QString());

    QTest::failOnWarning(QRegularExpression(u"outside its collection"_s));
    auto job = new KDAV::DavItemCreateJob(item);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->item().url().url().path(), u"/user@example.com/renamed.ics"_s);
}

QTEST_MAIN(DavItemCreateJobTest)

#include "moc_davitemcreatejobtest.cpp"
