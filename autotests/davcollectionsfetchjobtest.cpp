// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectionsfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollection>
#include <KDAV/DavCollectionsFetchJob>
#include <KDAV/DavError>

#include <QColor>
#include <QFile>
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
    QCOMPARE(collection.syncToken(), u"3142"_s);
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
    QCOMPARE(collection.syncToken(), u"3143"_s);
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
    QCOMPARE(collection.syncToken(), u"3140"_s);

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

// A PROPFIND on a collection, answered with the given HTTP status and response body
static QList<QByteArray> failingCollectionFetchScenario(const QByteArray &path, const QByteArray &status, const QByteArray &responseBody = {})
{
    QList<QByteArray> scenario{"C: PROPFIND " + path + " HTTP/1.1"};
    QFile bodyFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-propfind-body.txt"_s);
    if (!bodyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qFatal("Failed to read the PROPFIND body");
    }
    while (!bodyFile.atEnd()) {
        scenario << bodyFile.readLine().trimmed();
    }
    scenario << "S: HTTP/1.1 " + status;
    if (!responseBody.isEmpty()) {
        scenario << "D: " + responseBody;
    }
    scenario << "X";
    return scenario;
}

static KDAV::DavCollectionsFetchJob *runCalDavFetchJob(const FakeServer &fakeServer)
{
    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    auto job = new KDAV::DavCollectionsFetchJob(KDAV::DavUrl(url, KDAV::CalDav));
    job->exec();
    return job;
}

void DavCollectionsFetchJobTest::homeSetFetchError_data()
{
    QTest::addColumn<QByteArray>("status");

    // The server just advertised the home set, so any failure to read it is a server inconsistency: the job fails and can be retried later
    QTest::newRow("403") << QByteArray("403 Forbidden");
    QTest::newRow("404") << QByteArray("404 Not Found");
    QTest::newRow("500") << QByteArray("500 Internal Server Error");
    QTest::newRow("503") << QByteArray("503 Service Unavailable");
}

void DavCollectionsFetchJobTest::homeSetFetchError()
{
    QFETCH(QByteArray, status);
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL fails → job fails, no fetch on the original URL
    fakeServer.addScenario(failingCollectionFetchScenario("/caldav/dfaure%40example.com/", status));
    fakeServer.startAndWait();

    auto job = runCalDavFetchJob(fakeServer);

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), status.left(3).toInt());
    QVERIFY(job->canRetryLater());
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::homeSetAnswersGarbage()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL gets an HTML page instead of a multistatus → job fails, retryable
    fakeServer.addScenario(failingCollectionFetchScenario("/caldav/dfaure%40example.com/", "200 OK", "<html><body>Maintenance</body></html>"));
    fakeServer.startAndWait();

    auto job = runCalDavFetchJob(fakeServer);

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_COLLECTIONFETCH));
    QVERIFY(job->canRetryLater());
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::oneOfTwoHomeSetsFails()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns two home sets
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-two-homesets.txt"_s);
    // Round 2: the first home set has collections, the second one fails → the list would be incomplete, so the job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav-collections.txt"_s);
    fakeServer.addScenario(failingCollectionFetchScenario("/caldav/shared/", "404 Not Found"));
    fakeServer.startAndWait();

    auto job = runCalDavFetchJob(fakeServer);

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 404);
    QVERIFY(job->canRetryLater());
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::homeSetIsConfiguredUrl()
{
    FakeServer fakeServer;

    // Round 1: principal fetch succeeds and returns the configured URL as home set
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-homeset-is-configured-url.txt"_s);
    // Round 2: the home set fetch returns 404 → job fails
    fakeServer.addScenario(failingCollectionFetchScenario("/caldav", "404 Not Found"));
    fakeServer.startAndWait();

    auto job = runCalDavFetchJob(fakeServer);

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), int(KDAV::ERR_PROBLEM_WITH_REQUEST));
    QCOMPARE(job->latestResponseCode(), 404);
    QVERIFY(job->canRetryLater());
    QCOMPARE(job->collections().count(), 0);
}

QTEST_MAIN(DavCollectionsFetchJobTest)

#include "moc_davcollectionsfetchjobtest.cpp"
