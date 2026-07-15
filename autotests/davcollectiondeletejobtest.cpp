// SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davcollectiondeletejobtest.h"
#include "fakeserver.h"

#include <KDAV/DavCollectionDeleteJob>
#include <KDAV/DavError>
#include <KDAV/DavPushDontNotify>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavCollectionDeleteJobTest::deleteSucceeds_data()
{
    QTest::addColumn<std::optional<KDAV::DavPushDontNotify>>("davPushDontNotify");

    QTest::newRow("all-notification") << std::optional<KDAV::DavPushDontNotify>();
    QTest::newRow("no-notification") << std::optional(KDAV::DavPushDontNotify::ignoreAll());
    QTest::newRow("some-notification") << std::optional(
        KDAV::DavPushDontNotify::ignoreUrls({u"https://example.com/webdav/subscriptions/TOKEN1"_s, u"https://example.com/webdav/subscriptions/TOKEN2"_s}));
}

void DavCollectionDeleteJobTest::deleteSucceeds()
{
    QFETCH(std::optional<KDAV::DavPushDontNotify>, davPushDontNotify);

    FakeServer fakeServer;
    auto scenario = QByteArrayList() << "C: DELETE /collection HTTP/1.1";
    if (davPushDontNotify) {
        scenario << (u"C: "_s + davPushDontNotify->davHeader()).toUtf8();
    }
    scenario << "S: HTTP/1.1 200 OK"
             << "S: Date: Mon, 21 Apr 2026 15:00:00 GMT"
             << "X";
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavCollectionDeleteJob(davUrl);
    if (davPushDontNotify) {
        job->setPushDontNotify(*davPushDontNotify);
    }
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
}

void DavCollectionDeleteJobTest::deleteServerError()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /collection HTTP/1.1",
        "S: HTTP/1.1 403 Forbidden",
        "X",
    });
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/collection"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CardDav);

    auto job = new KDAV::DavCollectionDeleteJob(davUrl);
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QVERIFY(job->error() != 0);
    QCOMPARE(job->error(), static_cast<int>(KDAV::ERR_COLLECTIONDELETE));
    QCOMPARE(job->latestResponseCode(), 403);
}

QTEST_MAIN(DavCollectionDeleteJobTest)

#include "moc_davcollectiondeletejobtest.cpp"
