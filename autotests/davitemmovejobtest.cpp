// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davitemmovejobtest.h"

#include "fakeserver.h"

#include <KDAV/DavItem>
#include <KDAV/DavItemMoveJob>
#include <KDAV/DavPushDontNotify>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavItemMoveJobTest::moveSucceeds_data()
{
    QTest::addColumn<std::optional<KDAV::DavPushDontNotify>>("davPushDontNotify");

    QTest::newRow("all-notification") << std::optional<KDAV::DavPushDontNotify>();
    QTest::newRow("no-notification") << std::optional(KDAV::DavPushDontNotify::ignoreAll());
    QTest::newRow("some-notification") << std::optional(
        KDAV::DavPushDontNotify::ignoreUrls({u"https://example.com/webdav/subscriptions/TOKEN1"_s, u"https://example.com/webdav/subscriptions/TOKEN2"_s}));
}

void DavItemMoveJobTest::moveSucceeds()
{
    QFETCH(std::optional<KDAV::DavPushDontNotify>, davPushDontNotify);

    auto scenario = QByteArrayList() << "C: MOVE /caldav/old-calendar/item.ics HTTP/1.1"
                                     << "C: Destination: http://localhost/caldav/new-calendar/item.ics"
                                     << "C: Overwrite: T";
    if (davPushDontNotify) {
        scenario << (u"C: "_s + davPushDontNotify->davHeader()).toUtf8();
    }
    scenario << "S: HTTP/1.1 201 Created"
             << "S: Location: http://localhost/caldav/new-calendar/item.ics"
             << "X";

    FakeServer fakeServer;
    fakeServer.addScenario(scenario);
    fakeServer.startAndWait();

    auto itemUrl = QUrl("http://localhost/caldav/old-calendar/item.ics"_L1);
    itemUrl.setPort(fakeServer.port());
    auto davUrl = KDAV::DavUrl(itemUrl, KDAV::CalDav);
    auto item = KDAV::DavItem(davUrl, QString(), QByteArray(), u"\"test-etag\""_s);

    auto newItemUrl = QUrl("http://localhost/caldav/new-calendar/item.ics"_L1);
    auto job = new KDAV::DavItemMoveJob(item, newItemUrl);
    if (davPushDontNotify) {
        job->setPushDontNotify(*davPushDontNotify);
    }
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QVERIFY(!job->hasConflict());
}

QTEST_MAIN(DavItemMoveJobTest)

#include "moc_davitemmovejobtest.cpp"
