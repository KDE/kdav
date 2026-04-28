// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davitemsfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavError>
#include <KDAV/DavItem>
#include <KDAV/DavItemsFetchJob>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavItemsFetchJobTest::fetchCardDavSingleItem()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: REPORT /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <addressbook-multiget xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:  <prop xmlns=\"DAV:\">",
        "B:   <getetag xmlns=\"DAV:\"/>",
        "B:   <address-data xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:    <allprop xmlns=\"DAV:\"/>",
        "B:   </address-data>",
        "B:  </prop>",
        "B:  <href xmlns=\"DAV:\">/collection/item1.vcf</href>",
        "B: </addressbook-multiget>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>",
        "D: <multistatus xmlns=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:carddav\">",
        "D:   <response>",
        "D:     <href>/collection/item1.vcf</href>",
        "D:     <propstat>",
        "D:       <prop>",
        "D:         <getetag>\"etag-abc123\"</getetag>",
        "D:         <C:address-data>BEGIN:VCARD",
        "D: VERSION:3.0",
        "D: UID:12345678-1234-1234-1234-123456789abc",
        "D: FN:John Doe",
        "D: END:VCARD",
        "D: </C:address-data>",
        "D:       </prop>",
        "D:       <status>HTTP/1.1 200 OK</status>",
        "D:     </propstat>",
        "D:   </response>",
        "D: </multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl collectionUrl(url, KDAV::CardDav);

    QUrl itemUrl(url);
    itemUrl.setPath(u"/collection/item1.vcf"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {itemUrl.toString()});
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavItem::List items = job->items();
    QCOMPARE(items.size(), 1);

    const KDAV::DavItem item = job->item(itemUrl.toString());
    QCOMPARE(item.etag(), u"\"etag-abc123\""_s);
    QVERIFY(item.data().contains("BEGIN:VCARD"));
    QVERIFY(item.data().contains("FN:John Doe"));
    QCOMPARE(item.url().url().path(), u"/collection/item1.vcf"_s);
}

void DavItemsFetchJobTest::fetchCardDavTwoItems()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: REPORT /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <addressbook-multiget xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:  <prop xmlns=\"DAV:\">",
        "B:   <getetag xmlns=\"DAV:\"/>",
        "B:   <address-data xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:    <allprop xmlns=\"DAV:\"/>",
        "B:   </address-data>",
        "B:  </prop>",
        "B:  <href xmlns=\"DAV:\">/collection/item1.vcf</href>",
        "B:  <href xmlns=\"DAV:\">/collection/item2.vcf</href>",
        "B: </addressbook-multiget>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>",
        "D: <multistatus xmlns=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:carddav\">",
        "D:   <response>",
        "D:     <href>/collection/item1.vcf</href>",
        "D:     <propstat>",
        "D:       <prop>",
        "D:         <getetag>\"etag-item1\"</getetag>",
        "D:         <C:address-data>BEGIN:VCARD",
        "D: VERSION:3.0",
        "D: UID:aaaa-1111",
        "D: FN:Alice Smith",
        "D: END:VCARD",
        "D: </C:address-data>",
        "D:       </prop>",
        "D:       <status>HTTP/1.1 200 OK</status>",
        "D:     </propstat>",
        "D:   </response>",
        "D:   <response>",
        "D:     <href>/collection/item2.vcf</href>",
        "D:     <propstat>",
        "D:       <prop>",
        "D:         <getetag>\"etag-item2\"</getetag>",
        "D:         <C:address-data>BEGIN:VCARD",
        "D: VERSION:3.0",
        "D: UID:bbbb-2222",
        "D: FN:Bob Jones",
        "D: END:VCARD",
        "D: </C:address-data>",
        "D:       </prop>",
        "D:       <status>HTTP/1.1 200 OK</status>",
        "D:     </propstat>",
        "D:   </response>",
        "D: </multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl collectionUrl(url, KDAV::CardDav);

    QUrl item1Url(url);
    item1Url.setPath(u"/collection/item1.vcf"_s);
    QUrl item2Url(url);
    item2Url.setPath(u"/collection/item2.vcf"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {item1Url.toString(), item2Url.toString()});
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavItem::List items = job->items();
    QCOMPARE(items.size(), 2);

    const KDAV::DavItem item1 = job->item(item1Url.toString());
    QCOMPARE(item1.etag(), u"\"etag-item1\""_s);
    QVERIFY(item1.data().contains("FN:Alice Smith"));

    const KDAV::DavItem item2 = job->item(item2Url.toString());
    QCOMPARE(item2.etag(), u"\"etag-item2\""_s);
    QVERIFY(item2.data().contains("FN:Bob Jones"));
}

void DavItemsFetchJobTest::fetchCardDavServerError()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: REPORT /collection HTTP/1.1",
        "S: HTTP/1.1 403 Forbidden",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl collectionUrl(url, KDAV::CardDav);

    QUrl itemUrl(url);
    itemUrl.setPath(u"/collection/item1.vcf"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {itemUrl.toString()});
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 403);
}

void DavItemsFetchJobTest::fetchCardDavSkipsNonOkPropstat()
{
    // When a propstat has a non-200 status, the item is silently skipped.
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: REPORT /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <addressbook-multiget xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:  <prop xmlns=\"DAV:\">",
        "B:   <getetag xmlns=\"DAV:\"/>",
        "B:   <address-data xmlns=\"urn:ietf:params:xml:ns:carddav\">",
        "B:    <allprop xmlns=\"DAV:\"/>",
        "B:   </address-data>",
        "B:  </prop>",
        "B:  <href xmlns=\"DAV:\">/collection/item1.vcf</href>",
        "B: </addressbook-multiget>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>",
        "D: <multistatus xmlns=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:carddav\">",
        "D:   <response>",
        "D:     <href>/collection/item1.vcf</href>",
        "D:     <propstat>",
        "D:       <prop>",
        "D:         <getetag>\"etag-abc123\"</getetag>",
        "D:         <C:address-data>BEGIN:VCARD",
        "D: VERSION:3.0",
        "D: UID:12345678-1234-1234-1234-123456789abc",
        "D: FN:John Doe",
        "D: END:VCARD",
        "D: </C:address-data>",
        "D:       </prop>",
        "D:       <status>HTTP/1.1 404 Not Found</status>",
        "D:     </propstat>",
        "D:   </response>",
        "D: </multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl collectionUrl(url, KDAV::CardDav);

    QUrl itemUrl(url);
    itemUrl.setPath(u"/collection/item1.vcf"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {itemUrl.toString()});
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->items().size(), 0);
}

void DavItemsFetchJobTest::fetchCalDavSingleItem()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: REPORT /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <calendar-multiget xmlns=\"urn:ietf:params:xml:ns:caldav\">",
        "B:  <prop xmlns=\"DAV:\">",
        "B:   <getetag xmlns=\"DAV:\"/>",
        "B:   <calendar-data xmlns=\"urn:ietf:params:xml:ns:caldav\"/>",
        "B:  </prop>",
        "B:  <href xmlns=\"DAV:\">/collection/event1.ics</href>",
        "B: </calendar-multiget>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>",
        "D: <multistatus xmlns=\"DAV:\" xmlns:C=\"urn:ietf:params:xml:ns:caldav\">",
        "D:   <response>",
        "D:     <href>/collection/event1.ics</href>",
        "D:     <propstat>",
        "D:       <prop>",
        "D:         <getetag>\"etag-cal001\"</getetag>",
        "D:         <C:calendar-data>BEGIN:VCALENDAR",
        "D: VERSION:2.0",
        "D: BEGIN:VEVENT",
        "D: UID:event-uid-001",
        "D: SUMMARY:Team Meeting",
        "D: END:VEVENT",
        "D: END:VCALENDAR",
        "D: </C:calendar-data>",
        "D:       </prop>",
        "D:       <status>HTTP/1.1 200 OK</status>",
        "D:     </propstat>",
        "D:   </response>",
        "D: </multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl collectionUrl(url, KDAV::CalDav);

    QUrl itemUrl(url);
    itemUrl.setPath(u"/collection/event1.ics"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {itemUrl.toString()});
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavItem::List items = job->items();
    QCOMPARE(items.size(), 1);

    const KDAV::DavItem item = job->item(itemUrl.toString());
    QCOMPARE(item.etag(), u"\"etag-cal001\""_s);
    QVERIFY(item.data().contains("BEGIN:VCALENDAR"));
    QVERIFY(item.data().contains("SUMMARY:Team Meeting"));
    QCOMPARE(item.url().url().path(), u"/collection/event1.ics"_s);
}

void DavItemsFetchJobTest::fetchFailsForProtocolWithoutMultiget()
{
    QUrl url(u"http://localhost/collection"_s);
    KDAV::DavUrl collectionUrl(url, KDAV::GroupDav);

    QUrl itemUrl(url);
    itemUrl.setPath(u"/collection/item1.vcf"_s);

    auto job = new KDAV::DavItemsFetchJob(collectionUrl, {itemUrl.toString()});
    job->exec();

    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_NO_MULTIGET));
}

QTEST_MAIN(DavItemsFetchJobTest)

#include "moc_davitemsfetchjobtest.cpp"
