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
    FakeServer fakeServer;
    fakeServer.startAndWait();
    QUrl url(u"http://localhost/caldav/new-calendar/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    KDAV::DavCollection collection(davUrl, u"Test 2"_s, KDAV::DavCollection::Events);
    collection.setColor(QColor(u"#B6469D"_s));
    collection.setTimeZone(s_berlinTimezone);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenario({
        "C: MKCOL /caldav/new-calendar/ HTTP/1.1",
        "B: <?xml version=\"1.0\"?>",
        "B: <D:mkcol xmlns:D=\"DAV:\" xmlns:I=\"http://apple.com/ns/ical/\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\">",
        "B:     <D:set>",
        "B:         <D:prop>",
        "B:             <D:resourcetype>",
        "B:                 <D:collection/>",
        "B:                 <C:calendar/>",
        "B:             </D:resourcetype>",
        "B:             <D:displayname>Test 2</D:displayname>",
        "B:             <I:calendar-color>#ffb6469d</I:calendar-color>",
        "B:             <C:supported-calendar-component-set>",
        "B:                 <C:comp name=\"VEVENT\"/>",
        "B:             </C:supported-calendar-component-set>",
        "B:             <C:calendar-timezone>BEGIN:VCALENDAR",
        "B: PRODID:-//IDN nextcloud.com//Calendar app 6.3.0-rc.1//EN",
        "B: CALSCALE:GREGORIAN",
        "B: VERSION:2.0",
        "B: BEGIN:VTIMEZONE",
        "B: TZID:Europe/Berlin",
        "B: BEGIN:DAYLIGHT",
        "B: TZNAME:CEST",
        "B: TZOFFSETFROM:+0100",
        "B: TZOFFSETTO:+0200",
        "B: DTSTART:19700329T020000",
        "B: RRULE:FREQ=YEARLY;BYMONTH=3;BYDAY=-1SU",
        "B: END:DAYLIGHT",
        "B: BEGIN:STANDARD",
        "B: TZNAME:CET",
        "B: TZOFFSETFROM:+0200",
        "B: TZOFFSETTO:+0100",
        "B: DTSTART:19701025T030000",
        "B: RRULE:FREQ=YEARLY;BYMONTH=10;BYDAY=-1SU",
        "B: END:STANDARD",
        "B: END:VTIMEZONE",
        "B: END:VCALENDAR</C:calendar-timezone>",
        "B:         </D:prop>",
        "B:     </D:set>",
        "B: </D:mkcol>",
        "S: HTTP/1.0 201 Created",
        "X",
    });
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
    FakeServer fakeServer;
    fakeServer.startAndWait();
    QUrl url(u"http://localhost/carddav/new-addressbook/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    KDAV::DavCollection collection(davUrl, u"My Address Book"_s, KDAV::DavCollection::Contacts);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenario({
        "C: MKCOL /carddav/new-addressbook/ HTTP/1.1",
        "B: <?xml version=\"1.0\"?>",
        "B: <D:mkcol xmlns:D=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:carddav\">",
        "B:     <D:set>",
        "B:         <D:prop>",
        "B:             <D:resourcetype>",
        "B:                 <D:collection/>",
        "B:                 <C:addressbook/>",
        "B:             </D:resourcetype>",
        "B:             <D:displayname>My Address Book</D:displayname>",
        "B:         </D:prop>",
        "B:     </D:set>",
        "B: </D:mkcol>",
        "S: HTTP/1.0 201 Created",
        "X",
    });
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->collection().displayName(), u"My Address Book"_s);
    QCOMPARE(job->collection().contentTypes(), KDAV::DavCollection::Contacts);
    QCOMPARE(job->collectionUrl().path(), u"/carddav/new-addressbook/"_s);
}

void DavCollectionCreateJobTest::createCollectionError()
{
    FakeServer fakeServer;
    fakeServer.startAndWait();
    QUrl url(u"http://localhost/caldav/new-calendar/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    KDAV::DavCollection collection(davUrl, u"Test 2"_s, KDAV::DavCollection::Events);

    auto job = new KDAV::DavCollectionCreateJob(collection);

    fakeServer.addScenario({
        "C: MKCOL /caldav/new-calendar/ HTTP/1.1",
        "B: <?xml version=\"1.0\"?>",
        "B: <D:mkcol xmlns:D=\"DAV:\" xmlns:I=\"http://apple.com/ns/ical/\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\">",
        "B:     <D:set>",
        "B:         <D:prop>",
        "B:             <D:resourcetype>",
        "B:                 <D:collection/>",
        "B:                 <C:calendar/>",
        "B:             </D:resourcetype>",
        "B:             <D:displayname>Test 2</D:displayname>",
        "B:             <C:supported-calendar-component-set>",
        "B:                 <C:comp name=\"VEVENT\"/>",
        "B:             </C:supported-calendar-component-set>",
        "B:         </D:prop>",
        "B:     </D:set>",
        "B: </D:mkcol>",
        "S: HTTP/1.1 404 Not Found",
        "X",
    });
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
}

QTEST_MAIN(DavCollectionCreateJobTest)

#include "moc_davcollectioncreatejobtest.cpp"
