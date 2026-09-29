// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushunregistrationjobtest.h"

#include "common/daverror.h"
#include "fakeserver.h"

#include <KDAV/DavPushUnregistrationJob>
#include <KDAV/DavUrl>

#include <QTest>

using namespace Qt::StringLiterals;

void DavPushRegistrationJobTest::CalDavPushUnregistration_normal()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /apps/dav_push/subscriptions/4 HTTP/1.1",
        "S: HTTP/1.0 204 No Content",
        "X",
    });
    fakeServer.startAndWait();

    auto davUrl = QUrl(u"http://localhost/caldav/mycalendar/"_s);
    davUrl.setPort(fakeServer.port());
    const auto davCollectionUrl = KDAV::DavUrl(davUrl, KDAV::CalDav);
    auto registrationUrl = QUrl(u"http://localhost/apps/dav_push/subscriptions/4"_s);
    registrationUrl.setPort(fakeServer.port());

    auto *job = new KDAV::DavPushUnregistrationJob(davCollectionUrl, registrationUrl);

    job->exec();
    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
}

void DavPushRegistrationJobTest::CalDavPushUnregistration_missing()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /apps/dav_push/subscriptions/4 HTTP/1.1",
        "S: HTTP/1.0 404 Not Found",
        "X",
    });
    fakeServer.startAndWait();

    auto davUrl = QUrl(u"http://localhost/caldav/mycalendar/"_s);
    davUrl.setPort(fakeServer.port());
    const auto davCollectionUrl = KDAV::DavUrl(davUrl, KDAV::CalDav);
    auto registrationUrl = QUrl(u"http://localhost/apps/dav_push/subscriptions/4"_s);
    registrationUrl.setPort(fakeServer.port());

    auto *job = new KDAV::DavPushUnregistrationJob(davCollectionUrl, registrationUrl);

    job->exec();
    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
}

void DavPushRegistrationJobTest::CalDavPushUnregistration_fail()
{
    FakeServer fakeServer;
    fakeServer.addScenario({
        "C: DELETE /apps/dav_push/subscriptions/4 HTTP/1.1",
        "S: HTTP/1.0 403 Forbidden",
        "X",
    });
    fakeServer.startAndWait();

    auto davUrl = QUrl(u"http://localhost/caldav/mycalendar/"_s);
    davUrl.setPort(fakeServer.port());
    const auto davCollectionUrl = KDAV::DavUrl(davUrl, KDAV::CalDav);
    auto registrationUrl = QUrl(u"http://localhost/apps/dav_push/subscriptions/4"_s);
    registrationUrl.setPort(fakeServer.port());

    auto *job = new KDAV::DavPushUnregistrationJob(davCollectionUrl, registrationUrl);

    job->exec();
    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), KDAV::ERR_DAVPUSH_UNREGISTER);
}

QTEST_MAIN(DavPushRegistrationJobTest)

#include "moc_davpushunregistrationjobtest.cpp"
