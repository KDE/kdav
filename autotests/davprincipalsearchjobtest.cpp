// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davprincipalsearchjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavPrincipalSearchJob>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

QTEST_MAIN(DavPrincipalSearchJobTest)

void DavPrincipalSearchJobTest::initTestCase()
{
    qputenv("KIO_DISABLE_CACHE_CLEANER", "yes");
}

void DavPrincipalSearchJobTest::searchByDisplayName()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-success.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-report-displayname.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->results().count(), 1);
    QCOMPARE(job->results().at(0).propertyNamespace, u"DAV:"_s);
    QCOMPARE(job->results().at(0).property, u"displayname"_s);
    QCOMPARE(job->results().at(0).value, u"John Doe"_s);
}

void DavPrincipalSearchJobTest::searchByEmailAddress()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-success.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-report-emailaddress.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::EmailAddress, u"john@example.com"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->results().count(), 1);
    QCOMPARE(job->results().at(0).value, u"John Doe"_s);
}

void DavPrincipalSearchJobTest::propfindHttpError()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-error.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
}

void DavPrincipalSearchJobTest::propfindEmptyMultistatus()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-empty.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(job->results().isEmpty());
}

void DavPrincipalSearchJobTest::propfindNoPropstat200()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-no200.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(job->results().isEmpty());
}

void DavPrincipalSearchJobTest::reportHttpError()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-success.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-report-error.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
}

void DavPrincipalSearchJobTest::multipleCollectionSets()
{
    FakeServer fakeServer;
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-propfind-two-hrefs.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-report-users.txt"_s);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davprincipalsearchjob-report-groups.txt"_s);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/dav/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavPrincipalSearchJob(davUrl, KDAV::DavPrincipalSearchJob::DisplayName, u"John"_s);
    job->fetchProperty(u"displayname"_s, u"DAV:"_s);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->results().count(), 2);

    const QStringList values = {job->results().at(0).value, job->results().at(1).value};
    QVERIFY(values.contains(u"John Doe"_s));
    QVERIFY(values.contains(u"Engineering"_s));
}
