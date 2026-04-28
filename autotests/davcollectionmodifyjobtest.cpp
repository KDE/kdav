// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectionmodifyjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollectionModifyJob>
#include <KDAV/DavError>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavCollectionModifyJobTest::modifySucceeds()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: PROPPATCH /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <propertyupdate xmlns=\"DAV:\">",
        "B:  <set xmlns=\"DAV:\">",
        "B:   <prop xmlns=\"DAV:\">",
        "B:    <displayname>My Calendar</displayname>",
        "B:   </prop>",
        "B:  </set>",
        "B: </propertyupdate>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "D: <D:multistatus xmlns:D=\"DAV:\">",
        "D:  <D:response>",
        "D:   <D:href>/collection</D:href>",
        "D:   <D:propstat>",
        "D:    <D:prop><D:displayname/></D:prop>",
        "D:    <D:status>HTTP/1.1 200 OK</D:status>",
        "D:   </D:propstat>",
        "D:  </D:response>",
        "D: </D:multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionModifyJob(davUrl);
    job->setProperty(u"displayname"_s, u"My Calendar"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
}

void DavCollectionModifyJobTest::modifyServerError()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: PROPPATCH /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <propertyupdate xmlns=\"DAV:\">",
        "B:  <set xmlns=\"DAV:\">",
        "B:   <prop xmlns=\"DAV:\">",
        "B:    <displayname>My Calendar</displayname>",
        "B:   </prop>",
        "B:  </set>",
        "B: </propertyupdate>",
        "S: HTTP/1.1 403 Forbidden",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionModifyJob(davUrl);
    job->setProperty(u"displayname"_s, u"My Calendar"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_COLLECTIONMODIFY));
    QCOMPARE(job->latestResponseCode(), 403);
}

void DavCollectionModifyJobTest::modifyResponseError()
{
    FakeServer fakeServer;
    // Server returns 207 but with a non-200 propstat status
    fakeServer.addScenario({
        "C: PROPPATCH /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <propertyupdate xmlns=\"DAV:\">",
        "B:  <set xmlns=\"DAV:\">",
        "B:   <prop xmlns=\"DAV:\">",
        "B:    <displayname>My Calendar</displayname>",
        "B:   </prop>",
        "B:  </set>",
        "B: </propertyupdate>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "D: <D:multistatus xmlns:D=\"DAV:\">",
        "D:  <D:response>",
        "D:   <D:href>/collection</D:href>",
        "D:   <D:propstat>",
        "D:    <D:prop><D:displayname/></D:prop>",
        "D:    <D:status>HTTP/1.1 403 Forbidden</D:status>",
        "D:   </D:propstat>",
        "D:   <D:responsedescription>Property is read-only</D:responsedescription>",
        "D:  </D:response>",
        "D: </D:multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionModifyJob(davUrl);
    job->setProperty(u"displayname"_s, u"My Calendar"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_COLLECTIONMODIFY_RESPONSE));
}

void DavCollectionModifyJobTest::modifyNoProperties()
{
    // No property set or removed => should fail immediately without contacting server
    QUrl url(u"http://localhost/collection"_s);
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionModifyJob(davUrl);
    job->exec();

    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_COLLECTIONMODIFY_NO_PROPERITES));
}

void DavCollectionModifyJobTest::removePropertySucceeds()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: PROPPATCH /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <propertyupdate xmlns=\"DAV:\">",
        "B:  <remove xmlns=\"DAV:\">",
        "B:   <prop xmlns=\"DAV:\">",
        "B:    <displayname xmlns=\"DAV:\"/>",
        "B:   </prop>",
        "B:  </remove>",
        "B: </propertyupdate>",
        "S: HTTP/1.1 207 Multi-Status",
        "S: Content-Type: application/xml; charset=utf-8",
        "D: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "D: <D:multistatus xmlns:D=\"DAV:\">",
        "D:  <D:response>",
        "D:   <D:href>/collection</D:href>",
        "D:   <D:propstat>",
        "D:    <D:prop><D:displayname/></D:prop>",
        "D:    <D:status>HTTP/1.1 200 OK</D:status>",
        "D:   </D:propstat>",
        "D:  </D:response>",
        "D: </D:multistatus>",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionModifyJob(davUrl);
    job->removeProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
}

QTEST_MAIN(DavCollectionModifyJobTest)

#include "moc_davcollectionmodifyjobtest.cpp"
