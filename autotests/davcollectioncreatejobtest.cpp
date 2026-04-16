// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectioncreatejobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollection>
#include <KDAV/DavCollectionCreateJob>

#include <QColor>
#include <QTest>

using namespace Qt::StringLiterals;

static const QByteArray s_berlinTimezone =
    "BEGIN:VCALENDAR\n"
    "PRODID:-//IDN nextcloud.com//Calendar app 6.3.0-rc.1//EN\n"
    "CALSCALE:GREGORIAN\n"
    "VERSION:2.0\n"
    "BEGIN:VTIMEZONE\n"
    "TZID:Europe/Berlin\n"
    "BEGIN:DAYLIGHT\n"
    "TZNAME:CEST\n"
    "TZOFFSETFROM:+0100\n"
    "TZOFFSETTO:+0200\n"
    "DTSTART:19700329T020000\n"
    "RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU\n"
    "END:DAYLIGHT\n"
    "BEGIN:STANDARD\n"
    "TZNAME:CET\n"
    "TZOFFSETFROM:+0200\n"
    "TZOFFSETTO:+0100\n"
    "DTSTART:19701025T030000\n"
    "RRULE:FREQ=YEARLY;BYMONTH=10;BYDAY=-1SU\n"
    "END:STANDARD\n"
    "END:VTIMEZONE\n"
    "END:VCALENDAR";

void DavCollectionCreateJobTest::createCalDavCollection()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/caldav/new-calendar/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    KDAV::DavCollection collection(davUrl, u"Test 2"_s, KDAV::DavCollection::Events);
    collection.setColor(QColor(u"#B6469D"_s));
    collection.setTimeZone(s_berlinTimezone);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/datacollectioncreatejob-caldav-success.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->collection().displayName(), u"Test 2"_s);
    QCOMPARE(job->collection().contentTypes(), KDAV::DavCollection::Events);
    QCOMPARE(job->collection().color(), QColor(u"#B6469D"_s));
    QCOMPARE(job->collectionUrl().path(), u"/caldav/new-calendar/"_s);
}

void DavCollectionCreateJobTest::createCardDavCollection()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/carddav/new-addressbook/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    KDAV::DavCollection collection(davUrl, u"My Address Book"_s, KDAV::DavCollection::Contacts);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/datacollectioncreatejob-carddav-success.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->collection().displayName(), u"My Address Book"_s);
    QCOMPARE(job->collection().contentTypes(), KDAV::DavCollection::Contacts);
    QCOMPARE(job->collectionUrl().path(), u"/carddav/new-addressbook/"_s);
}

void DavCollectionCreateJobTest::createCollectionError()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/caldav/new-calendar/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    KDAV::DavCollection collection(davUrl, u"Test 2"_s, KDAV::DavCollection::Events);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/datacollectioncreatejob-error.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
}

QTEST_MAIN(DavCollectionCreateJobTest)

#include "moc_davcollectioncreatejobtest.cpp"
