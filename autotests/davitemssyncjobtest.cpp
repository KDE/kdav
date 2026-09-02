/*
 *  SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "davitemssyncjobtest.h"
#include "KDAV/DavUrl"
#include "common/enums.h"
#include "fakeserver.h"

#include <KDAV/DavError>
#include <KDAV/DavItemsSyncJob>

#include <QTest>

using namespace Qt::StringLiterals;

namespace
{
std::tuple<QString, QString> defaultOldNewSyncToken()
{
    return {
        "http://example.com/ns/sync/1234"_L1,
        "http://example.com/ns/sync/1238"_L1,
    };
}

QList<QByteArray> defaultSyncCollectionRequest(const QString &syncToken)
{
    return {
        "C: REPORT /collection HTTP/1.1",
        "B: <?xml version=\"1.0\" encoding=\"utf-8\"?>",
        "B: <D:sync-collection xmlns:D=\"DAV:\">",
        "B:     <D:sync-token>" + syncToken.toUtf8() + "</D:sync-token>",
        "B:     <D:sync-level>1</D:sync-level>",
        "B:     <D:prop>",
        "B:         <D:getetag/>",
        "B:         <D:resourcetype/>",
        "B:     </D:prop>",
        "B: </D:sync-collection>",
    };
}

QList<QByteArray> defaultSyncCollectionResponseChange(const QByteArray &href, const QByteArray &etag)
{
    return {
        "D:     <D:response>",
        "D:         <D:href>" + href + "</D:href>",
        "D:         <D:propstat>",
        "D:             <D:prop>",
        "D:                 <D:getetag>" + etag + "</D:getetag>",
        "D:             </D:prop>",
        "D:             <D:status>HTTP/1.1 200 OK</D:status>",
        "D:         </D:propstat>",
        "D:         <D:propstat>",
        "D:             <D:prop>",
        "D:                 <R:bigbox xmlns:R=\"urn:ns.example.com:boxschema\"/>",
        "D:             </D:prop>",
        "D:             <D:status>HTTP/1.1 404 Not Found</D:status>",
        "D:         </D:propstat>",
        "D:     </D:response>",
    };
}

QList<QByteArray> defaultSyncCollectionResponseDelete(const QByteArray &href)
{
    return {
        "D:     <D:response>",
        "D:         <D:href>" + href + "</D:href>",
        "D:         <D:status>HTTP/1.1 404 Not Found</D:status>",
        "D:     </D:response>",
    };
}

QString absoluteSyncCollectionResponsePath(const FakeServer &fake_server, const QByteArray &href)
{
    return QString::fromUtf8(QByteArray("http://localhost:") + QByteArray::number(fake_server.port()) + href);
}

}

void DavItemsSyncJobTest::calDavSyncChangedItems()
{
    const auto [oldSyncToken, newSyncToken] = defaultOldNewSyncToken();
    const auto scenario = defaultSyncCollectionRequest(oldSyncToken)
        << "S: HTTP/1.1 207 Multi-Status"
        << "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        << "D: <D:multistatus xmlns:D=\"DAV:\">" << defaultSyncCollectionResponseChange("/collection/event1.ics", "etag-001")
        << defaultSyncCollectionResponseChange("/collection/event2.ics", "etag-002") << "D:     <D:sync-token>" + newSyncToken.toUtf8() + "</D:sync-token>"
        << "D: </D:multistatus>";
    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavItemsSyncJob(davUrl, oldSyncToken);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->changedItems().size(), 2);
    QCOMPARE(job->changedItems().at(0).url().toDisplayString(), absoluteSyncCollectionResponsePath(fakeServer, "/collection/event1.ics"));
    QCOMPARE(job->changedItems().at(0).etag(), "etag-001"_L1);
    QCOMPARE(job->changedItems().at(1).url().toDisplayString(), absoluteSyncCollectionResponsePath(fakeServer, "/collection/event2.ics"));
    QCOMPARE(job->changedItems().at(1).etag(), "etag-002"_L1);
    QCOMPARE(job->deletedItems().size(), 0);
    QCOMPARE(job->newSyncToken(), newSyncToken);
}

void DavItemsSyncJobTest::calDavSyncDeletedItems()
{
    const auto [oldSyncToken, newSyncToken] = defaultOldNewSyncToken();
    const auto scenario = defaultSyncCollectionRequest(oldSyncToken)
        << "S: HTTP/1.1 207 Multi-Status"
        << "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        << "D: <D:multistatus xmlns:D=\"DAV:\">" << defaultSyncCollectionResponseDelete("/collection/event1.ics")
        << defaultSyncCollectionResponseDelete("/collection/event2.ics") << "D:     <D:sync-token>" + newSyncToken.toUtf8() + "</D:sync-token>"
        << "D: </D:multistatus>";
    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavItemsSyncJob(davUrl, oldSyncToken);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->changedItems().size(), 0);
    QCOMPARE(job->deletedItems().size(), 2);
    QCOMPARE(job->deletedItems(),
             QStringList({absoluteSyncCollectionResponsePath(fakeServer, "/collection/event1.ics"),
                          absoluteSyncCollectionResponsePath(fakeServer, "/collection/event2.ics")}));
    QCOMPARE(job->newSyncToken(), newSyncToken);
}

void DavItemsSyncJobTest::calDavSyncChangedAndDeletedItems()
{
    const auto [oldSyncToken, newSyncToken] = defaultOldNewSyncToken();
    const auto scenario = defaultSyncCollectionRequest(oldSyncToken)
        << "S: HTTP/1.1 207 Multi-Status"
        << "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        << "D: <D:multistatus xmlns:D=\"DAV:\">" << defaultSyncCollectionResponseChange("/collection/event1.ics", "etag-001")
        << defaultSyncCollectionResponseDelete("/collection/event2.ics") << defaultSyncCollectionResponseDelete("/collection/event3.ics")
        << defaultSyncCollectionResponseChange("/collection/event4.ics", "etag-004") << "D:     <D:sync-token>" + newSyncToken.toUtf8() + "</D:sync-token>"
        << "D: </D:multistatus>";
    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavItemsSyncJob(davUrl, oldSyncToken);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->changedItems().size(), 2);
    QCOMPARE(job->changedItems().at(0).url().toDisplayString(), absoluteSyncCollectionResponsePath(fakeServer, "/collection/event1.ics"));
    QCOMPARE(job->changedItems().at(0).etag(), "etag-001"_L1);
    QCOMPARE(job->deletedItems().size(), 2);
    QCOMPARE(job->deletedItems(),
             QStringList({absoluteSyncCollectionResponsePath(fakeServer, "/collection/event2.ics"),
                          absoluteSyncCollectionResponsePath(fakeServer, "/collection/event3.ics")}));
    QCOMPARE(job->changedItems().at(1).url().toDisplayString(), absoluteSyncCollectionResponsePath(fakeServer, "/collection/event4.ics"));
    QCOMPARE(job->changedItems().at(1).etag(), "etag-004"_L1);
    QCOMPARE(job->newSyncToken(), newSyncToken);
}

void DavItemsSyncJobTest::calDavSyncMalformedMissingHref()
{
    const auto [oldSyncToken, newSyncToken] = defaultOldNewSyncToken();
    const auto scenario = defaultSyncCollectionRequest(oldSyncToken)
        << "S: HTTP/1.1 207 Multi-Status"
        << "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        << "D: <D:multistatus xmlns:D=\"DAV:\">"
        << "D:     <D:response>"
        << "D:         <D:propstat>"
        << "D:             <D:prop>"
        << "D:                 <D:getetag>etag-001</D:getetag>"
        << "D:             </D:prop>"
        << "D:             <D:status>HTTP/1.1 200 OK</D:status>"
        << "D:         </D:propstat>"
        << "D:     </D:response>"
        << "D:     <D:sync-token>" + newSyncToken.toUtf8() + "</D:sync-token>" << "D: </D:multistatus>";
    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavItemsSyncJob(davUrl, oldSyncToken);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), KDAV::ERR_MALFORMED_RESPONSE);
}

void DavItemsSyncJobTest::calDavSyncMalformedMissingPropstat()
{
    const auto [oldSyncToken, newSyncToken] = defaultOldNewSyncToken();
    const auto scenario = defaultSyncCollectionRequest(oldSyncToken)
        << "S: HTTP/1.1 207 Multi-Status"
        << "D: <?xml version=\"1.0\" encoding=\"utf-8\" ?>"
        << "D: <D:multistatus xmlns:D=\"DAV:\">"
        << "D:     <D:response>"
        << "D:         <D:href>/collection/event2.ics</D:href>"
        << "D:     </D:response>"
        << "D:     <D:sync-token>" + newSyncToken.toUtf8() + "</D:sync-token>" << "D: </D:multistatus>";
    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavItemsSyncJob(davUrl, oldSyncToken);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), KDAV::ERR_MALFORMED_RESPONSE);
}

QTEST_MAIN(DavItemsSyncJobTest)

#include "moc_davitemssyncjobtest.cpp"
