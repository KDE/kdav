/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>
    SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemslistjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavError>
#include <KDAV/DavItemsListJob>
#include <KDAV/DavUrl>
#include <KDAV/EtagCache>

#include <QTest>

using namespace Qt::StringLiterals;

void DavItemsListJobTest::noMatchingMimetype()
{
    auto cache = std::make_shared<KDAV::EtagCache>();

    QUrl url(QStringLiteral("http://localhost/collection"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::Error error(KDAV::ErrorNumber::ERR_ITEMLIST_NOMIMETYPE, 0, QString(), 0);

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    job->setContentMimeTypes(QStringList() << QStringLiteral("mime/invalid1") << QStringLiteral("mime/invalid2"));
    job->exec();

    QCOMPARE(job->error(), static_cast<int>(KDAV::ErrorNumber::ERR_ITEMLIST_NOMIMETYPE));
    QCOMPARE(job->errorText(), error.errorText());
}

void DavItemsListJobTest::cardDavListingWithEmptyCache()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-items.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->items().count(), 2);
    // All items are new (empty cache), so all appear as changed
    QCOMPARE(job->changedItems().count(), 2);
    QCOMPARE(job->deletedItems().count(), 0);

    const auto items = job->items();
    const QStringList itemUrls = {
        QStringLiteral("http://localhost:%1/collection/item1.vcf").arg(fakeServer.port()),
        QStringLiteral("http://localhost:%1/collection/item2.vcf").arg(fakeServer.port()),
    };
    for (const auto &item : items) {
        QVERIFY(itemUrls.contains(item.url().url().toDisplayString()));
    }
}

void DavItemsListJobTest::cardDavListingWithUnchangedEtags()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();
    // Pre-populate cache with current ETags — items have not changed
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item1.vcf").arg(fakeServer.port()), u"\"etag1\""_s);
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item2.vcf").arg(fakeServer.port()), u"\"etag2\""_s);

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-items.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->items().count(), 2);
    QCOMPARE(job->changedItems().count(), 0);
    QCOMPARE(job->deletedItems().count(), 0);
}

void DavItemsListJobTest::cardDavListingWithChangedEtag()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();
    // item1 has an outdated ETag; item2 is current
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item1.vcf").arg(fakeServer.port()), u"\"old-etag1\""_s);
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item2.vcf").arg(fakeServer.port()), u"\"etag2\""_s);

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-items.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->items().count(), 2);
    QCOMPARE(job->changedItems().count(), 1);
    QCOMPARE(job->changedItems().at(0).url().url().path(), u"/collection/item1.vcf"_s);
    QCOMPARE(job->deletedItems().count(), 0);
}

void DavItemsListJobTest::cardDavListingDeletedItems()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();
    // Cache knows about two items, but the server will only return item1
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item1.vcf").arg(fakeServer.port()), u"\"etag1\""_s);
    cache->setEtag(QStringLiteral("http://localhost:%1/collection/item2.vcf").arg(fakeServer.port()), u"\"etag2\""_s);

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-single-item.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->items().count(), 1);
    QCOMPARE(job->changedItems().count(), 0);
    QCOMPARE(job->deletedItems().count(), 1);
    QCOMPARE(job->deletedItems().at(0), QStringLiteral("http://localhost:%1/collection/item2.vcf").arg(fakeServer.port()));
}

void DavItemsListJobTest::cardDavListingErrorResponse()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-error.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->items().count(), 0);
}

void DavItemsListJobTest::cardDavListingCollectionsSkipped()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-collection-and-item.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    // The collection entry in the response must be ignored; only the item is returned
    QCOMPARE(job->items().count(), 1);
    QCOMPARE(job->items().at(0).url().url().path(), u"/collection/item1.vcf"_s);
}

void DavItemsListJobTest::cardDavListingDuplicateUrls()
{
    FakeServer fakeServer(5992);
    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto cache = std::make_shared<KDAV::EtagCache>();

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR) + u"/davitemslistjob-carddav-duplicate.txt"_s);
    fakeServer.startAndWait();
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    // Duplicate URLs in the server response must be deduplicated
    QCOMPARE(job->items().count(), 1);
}

QTEST_MAIN(DavItemsListJobTest)

#include "moc_davitemslistjobtest.cpp"
