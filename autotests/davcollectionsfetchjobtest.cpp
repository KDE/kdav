// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectionsfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollection>
#include <KDAV/DavCollectionsFetchJob>

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
    FakeServer fakeServer(5991);
    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    // Round 1: DavPrincipalHomeSetsFetchJob fetches the principal home set
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: fetch the actual collections from the home set URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav-collections.txt"_s);
    fakeServer.startAndWait();
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
    QUrl url(u"http://localhost/carddav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-carddav.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-carddav-collections.txt"_s);
    fakeServer.startAndWait();
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
    FakeServer fakeServer(5991);
    QUrl url(u"http://localhost/groupdav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::GroupDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);
    QSignalSpy spy(job, &KDAV::DavCollectionsFetchJob::collectionDiscovered);

    // GroupDAV does not support principals — goes straight to a PROPFIND
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-groupdav.txt"_s);
    fakeServer.startAndWait();
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
    FakeServer fakeServer(5991);
    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    // Principal fetch (reuse existing scenario)
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Collection fetch with #RRGGBBAA color
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-caldav-color-collections.txt"_s);
    fakeServer.startAndWait();
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
    FakeServer fakeServer(5991);
    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-principal-error.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

void DavCollectionsFetchJobTest::collectionFetchError()
{
    FakeServer fakeServer(5991);
    QUrl url(u"http://localhost/caldav"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsFetchJob(davUrl);

    // Round 1: principal fetch succeeds and returns home set /caldav/dfaure%40example.com/
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/dataitemmultifetchjob-caldav.txt"_s);
    // Round 2: collection fetch on the home set URL returns 404 → triggers fallback to original URL
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-collection-homeset-404.txt"_s);
    // Round 3: fallback fetch on the original /caldav URL also returns 404 → job fails
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davcollectionsfetchjob-fallback-404.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->collections().count(), 0);
}

QTEST_MAIN(DavCollectionsFetchJobTest)

#include "moc_davcollectionsfetchjobtest.cpp"
