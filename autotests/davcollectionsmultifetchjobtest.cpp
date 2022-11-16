/*
    SPDX-FileCopyrightText: 2020 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davcollectionsmultifetchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollectionsMultiFetchJob>

#include <QSignalSpy>
#include <QTest>

using KDAV::DavCollection;
Q_DECLARE_METATYPE(KDAV::Protocol)

void DavCollectionsMultiFetchJobTest::initTestCase()
{
    // To let ctest exit, we shouldn't start kio_http_cache_cleaner
    qputenv("KIO_DISABLE_CACHE_CLEANER", "yes");

    qRegisterMetaType<KDAV::Protocol>();
}

void DavCollectionsMultiFetchJobTest::runSuccessfullTest()
{
    FakeServer fakeServer(5990);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl1(url, KDAV::CalDav);
    QUrl url2(url);
    url2.setPath(QStringLiteral("/carddav"));
    KDAV::DavUrl davUrl2(url2, KDAV::CardDav);

    auto job = new KDAV::DavCollectionsMultiFetchJob({davUrl1, davUrl2});

    QSignalSpy spy(job, &KDAV::DavCollectionsMultiFetchJob::collectionDiscovered);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav-collections.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-carddav.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-carddav-collections.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    const KDAV::DavCollection::List collections = job->collections();
    QCOMPARE(collections.count(), 2);
    int calDavIdx = 0;
    int cardDavIdx = 1;
    if (collections.at(0).contentTypes() == DavCollection::Contacts) { // put things in a defined order
        std::swap(calDavIdx, cardDavIdx);
    }

    const KDAV::DavCollection calendar = collections.at(calDavIdx);
    QCOMPARE(calendar.displayName(), QStringLiteral("Test1 User"));
    QCOMPARE(calendar.contentTypes(), DavCollection::Events | DavCollection::Todos | DavCollection::FreeBusy | DavCollection::Journal);
    QCOMPARE(calendar.url().url().path(), QStringLiteral("/caldav.php/test1.user/home/"));
    QCOMPARE(calendar.CTag(), QStringLiteral("12345"));
    QCOMPARE(calendar.privileges(), KDAV::Read);

    const KDAV::DavCollection addressbook = collections.at(cardDavIdx);
    QCOMPARE(addressbook.displayName(), QStringLiteral("My Address Book"));
    QCOMPARE(addressbook.contentTypes(), DavCollection::Contacts);
    QCOMPARE(addressbook.url().url().path(), QStringLiteral("/carddav.php/test1.user/home/"));
    QCOMPARE(addressbook.CTag(), QStringLiteral("3145"));
    QCOMPARE(addressbook.privileges(), KDAV::All);

    QCOMPARE(spy.count(), 2);
    QCOMPARE(int(spy.at(calDavIdx).at(0).value<KDAV::Protocol>()), int(KDAV::CalDav));
    QCOMPARE(spy.at(calDavIdx).at(1).toString(), calendar.url().url().toString());
    QCOMPARE(spy.at(calDavIdx).at(2).toString(), url.toString());

    QCOMPARE(int(spy.at(cardDavIdx).at(0).value<KDAV::Protocol>()), int(KDAV::CardDav));
    QCOMPARE(spy.at(cardDavIdx).at(1).toString(), addressbook.url().url().toString());
    QCOMPARE(spy.at(cardDavIdx).at(2).toString(), url2.toString());
}

void DavCollectionsMultiFetchJobTest::shouldFailOnError()
{
    FakeServer fakeServer(5990);
    QUrl url(QStringLiteral("http://localhost/caldav"));
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl1(url, KDAV::CalDav);
    QUrl urlError(url);
    urlError.setPath(QStringLiteral("/does_not_exist"));
    KDAV::DavUrl davUrlError(urlError, KDAV::CalDav);

    auto job = new KDAV::DavCollectionsMultiFetchJob({davUrl1, davUrlError});

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-caldav-collections.txt"));
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + QStringLiteral("/dataitemmultifetchjob-error.txt"));
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 300);
}

QTEST_MAIN(DavCollectionsMultiFetchJobTest)
