// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectionsfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollection>
#include <KDAV/DavCollectionsFetchJob>
#include <KDAV/DavError>

#include <QColor>
#include <QSignalSpy>
#include <QTest>

using namespace Qt::StringLiterals;

Q_DECLARE_METATYPE(KDAV::Protocol)

void DavCollectionsFetchJobTest::initTestCase()
{
    qRegisterMetaType<KDAV::Protocol>();
}

void DavCollectionsFetchJobTest::fetchCalDavCollections()
{
    FakeServer fakeServer;

    // Round 1: DavPrincipalHomeSetsFetchJob fetches the principal home set
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: fetch the actual collections from the home set URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav-collections.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->davUrl().url(), url);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), u"Test1 User"_s);
    QCOMPARE(collection.CTag(), u"12345"_s);
    QCOMPARE(collection.url().url().path(), u"/caldav.php/test1.user/home/"_s);
    QCOMPARE(collection.contentTypes(),
             KDAV::DavCollection::Events | KDAV::DavCollection::Todos | KDAV::DavCollection::FreeBusy | KDAV::DavCollection::Journal
                 | KDAV::DavCollection::Timezone);
    QCOMPARE(collection.privileges(), KDAV::Read);
    QVERIFY(!collection.color().isValid());

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::CalDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::fetchCardDavCollections()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-carddav.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-carddav-collections.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/carddav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), u"My Address Book"_s);
    QCOMPARE(collection.CTag(), u"3145"_s);
    QCOMPARE(collection.url().url().path(), u"/carddav.php/test1.user/home/"_s);
    QCOMPARE(collection.contentTypes(), KDAV::DavCollection::Contacts);
    QCOMPARE(collection.privileges(), KDAV::All);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::CardDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::fetchGroupDavCollections()
{
    FakeServer fakeServer;

    // GroupDAV does not support principals — goes straight to a PROPFIND
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-groupdav.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/groupdav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::GroupDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), u"My Events"_s);
    QCOMPARE(collection.url().url().path(), u"/groupdav/calendars/"_s);
    QCOMPARE(collection.contentTypes(), KDAV::DavCollection::Events);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::GroupDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::calendarWithColor()
{
    FakeServer fakeServer;
    // Principal fetch (reuse existing scenario)
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Collection fetch with #RRGGBBAA color
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-caldav-color-collections.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), u"Color Calendar"_s);
    QCOMPARE(collection.CTag(), u"99999"_s);

    QVERIFY(collection.color().isValid());
    QCOMPARE(collection.color(), QColor(255, 0, 0));
}

void DavCollectionsFetchJobTest::principalFetchError()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-error.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::collectionFetchError()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL returns 404 → triggers fallback to original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-404.txt"_s);
    // Round 3: fallback fetch on the original /caldav URL also returns 404 → job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-404.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::homeSetErrorWithEmptyFallback()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL returns 404 → triggers fallback to original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-404.txt"_s);
    // Round 3: fallback fetch on the original /caldav URL succeeds but lists no collection → the home set error must be reported,
    // otherwise the caller would take the empty list as the truth and delete its collections
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-empty.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 404);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::homeSetErrorWithFallbackCollection()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL returns 404 → triggers fallback to original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-404.txt"_s);
    // Round 3: fallback fetch on the original /caldav URL finds a collection → success
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-collection.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->collections().count(), 1);
    QCOMPARE(job->collections().at(0).displayName(), u"Fallback Calendar"_s);
}

void DavCollectionsFetchJobTest::twoHomeSetErrorsWithFallbackCollection()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns two home sets
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-two-homesets.txt"_s);
    // Round 2: both home set fetches return 404
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-404.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset2-404.txt"_s);
    // Round 3: the fallback fetch on the original /caldav URL runs once and finds a collection → success
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-collection.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->collections().count(), 1);
}

void DavCollectionsFetchJobTest::oneHomeSetError()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns two home sets
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-two-homesets.txt"_s);
    // Round 2: the first home set has collections, the second one returns 404 → the list is incomplete,
    // so the job must fail, and there is no point in a fallback fetch on the original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav-collections.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset2-404.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 404);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::homeSetTemporaryError()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL returns 503 → the home set is fine, so no fallback fetch: the job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-503.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 503);
    QVERIFY(job->canRetryLater());
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::goneAndTemporaryHomeSetErrors()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns two home sets
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-two-homesets.txt"_s);
    // Round 2: one home set returns 503, the other 404 → the 503 is reported, whatever the order of the replies, and there is no fallback
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-503.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset2-404.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 503);
    QVERIFY(job->canRetryLater());
}

void DavCollectionsFetchJobTest::homeSetIsConfiguredUrl()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns the configured URL as home set
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-homeset-is-configured-url.txt"_s);
    // Round 2: the home set fetch returns 404 → no point in fetching the same URL again, the job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-404.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 404);
}

QTEST_MAIN(DavCollectionsFetchJobTest)

#include "moc_davcollectionsfetchjobtest.cpp"
