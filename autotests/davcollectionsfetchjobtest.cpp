// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectionsfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollection>
#include <KDAV/DavCollectionsFetchJob>

#include <QColor>
#include <QSignalSpy>
#include <QTest>

Q_DECLARE_METATYPE(KDAV::Protocol)

void DavCollectionsFetchJobTest::initTestCase()
{
    qRegisterMetaType<KDAV::Protocol>();
}

void DavCollectionsFetchJobTest::fetchCalDavCollections()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    // Round 1: DavPrincipalHomeSetsFetchJob fetches the principal home set
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav.txt"));
    // Round 2: fetch the actual collections from the home set URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav-collections.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->davUrl().url(), url);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), QStringLiteral("Test1 User"));
    QCOMPARE(collection.CTag(), QStringLiteral("12345"));
    QCOMPARE(collection.url().url().path(), QStringLiteral("/caldav.php/test1.user/home/"));
    QCOMPARE(collection.contentTypes(),
             KDAV::DavCollection::Events | KDAV::DavCollection::Todos | KDAV::DavCollection::FreeBusy | KDAV::DavCollection::Journal);
    QCOMPARE(collection.privileges(), KDAV::Read);
    QVERIFY(!collection.color().isValid());

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::CalDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::fetchCardDavCollections()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/carddav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-carddav.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-carddav-collections.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), QStringLiteral("My Address Book"));
    QCOMPARE(collection.CTag(), QStringLiteral("3145"));
    QCOMPARE(collection.url().url().path(), QStringLiteral("/carddav.php/test1.user/home/"));
    QCOMPARE(collection.contentTypes(), KDAV::DavCollection::Contacts);
    QCOMPARE(collection.privileges(), KDAV::All);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::CardDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::fetchGroupDavCollections()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/groupdav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::GroupDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    // GroupDAV does not support principals — goes straight to a PROPFIND
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/davcollectionsfetchjob-groupdav.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), QStringLiteral("My Events"));
    QCOMPARE(collection.url().url().path(), QStringLiteral("/groupdav/calendars/"));
    QCOMPARE(collection.contentTypes(), KDAV::DavCollection::Events);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).value<KDAV::Protocol>(), KDAV::GroupDav);
    QCOMPARE(spy.at(0).at(1).toString(), collection.url().url().toString());
    QCOMPARE(spy.at(0).at(2).toString(), url.toString());
}

void DavCollectionsFetchJobTest::calendarWithColor()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    // Principal fetch (reuse existing scenario)
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav.txt"));
    // Collection fetch with #RRGGBBAA color
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/davcollectionsfetchjob-caldav-color-collections.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 1);

    const KDAV::DavCollection collection = collections.at(0);
    QCOMPARE(collection.displayName(), QStringLiteral("Color Calendar"));
    QCOMPARE(collection.CTag(), QStringLiteral("99999"));

    QVERIFY(collection.color().isValid());
    QCOMPARE(collection.color(), QColor(255, 0, 0));
}

void DavCollectionsFetchJobTest::principalFetchError()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/davcollectionsfetchjob-principal-error.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::collectionFetchError()
{
    FakeServer fakeServer(5991);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav.txt"));
    // Round 2: collection fetch on the home set URL returns 404 → triggers fallback to original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/davcollectionsfetchjob-collection-homeset-404.txt"));
    // Round 3: fallback fetch on the original /caldav URL also returns 404 → job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/davcollectionsfetchjob-fallback-404.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

QTEST_MAIN(DavCollectionsFetchJobTest)

#include "moc_davcollectionsfetchjobtest.cpp"
