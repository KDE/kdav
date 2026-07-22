// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushregistrationjobtest.h"
#include "common/davpushregistrationjob.h"

#include "KDAV/DavPushRegistration"
#include "KDAV/DavUrl"
#include "fakeserver.h"

#include <QTest>

using namespace Qt::StringLiterals;

void DavPushRegistrationJobTest::createCalDavPushRegistration()
{
    FakeServer fakeServer;
    fakeServer.startAndWait();

    QUrl url(u"http://localhost/caldav/mycalendar/"_s);
    url.setPort(fakeServer.port());
    KDAV::DavUrl davUrl(url, KDAV::CalDav);

    const auto pushEndpoint = QUrl::fromUserInput("https://up.example.net/yohd4yai5Phiz1wi"_L1);
    const auto subscriptionPublicKey = QByteArray("BCVxsr7N_eNgVRqvHtD0zTZsEc6-VV-JvLexhqUzORcxaOzi6-AYWXvTBHm4bjyPjs7Vd8pZGH6SRpkNtoIAiw4");
    const auto authToken = QByteArray("BTBZMqHH6r4Tts7J_aSIgg");
    const auto expiration = QDateTime::fromString("2026-08-01T12:30:00Z"_L1, Qt::ISODate);
    const auto job = new KDAV::DavPushRegistrationJob(davUrl, KDAV::DavPushRegistration(pushEndpoint, subscriptionPublicKey, authToken, expiration));

    fakeServer.addScenario({
        "C: POST /caldav/mycalendar/ HTTP/1.1",
        "C: Content-Type: application/xml",
        "B: <?xml version=\"1.0\"?>",
        "B: <push-register xmlns=\"https://bitfire.at/webdav-push\" xmlns:D=\"DAV:\" xmlns:I=\"http://apple.com/ns/ical/\">",
        "B:     <subscription>",
        "B:         <web-push-subscription>",
        "B:             <push-resource>https://up.example.net/yohd4yai5Phiz1wi</push-resource>",
        "B:             <content-encoding>aes128gcm</content-encoding>",
        "B:             <subscription-public-key "
        "type=\"p256dh\">BCVxsr7N_eNgVRqvHtD0zTZsEc6-VV-JvLexhqUzORcxaOzi6-AYWXvTBHm4bjyPjs7Vd8pZGH6SRpkNtoIAiw4</subscription-public-key>",
        "B:             <auth-secret>BTBZMqHH6r4Tts7J_aSIgg</auth-secret>",
        "B:         </web-push-subscription>",
        "B:     </subscription>",
        "B:     <trigger>",
        "B:         <content-update>",
        "B:             <D:depth>infinity</D:depth>",
        "B:         </content-update>",
        "B:         <property-update>",
        "B:             <D:depth>0</D:depth>",
        "B:             <D:prop>",
        "B:                 <D:displayname/>",
        "B:                 <I:calendar-color/>",
        "B:             </D:prop>",
        "B:         </property-update>",
        "B:     </trigger>",
        "B:     <expires>Sat, 01 Aug 2026 12:30:00 GMT</expires>",
        "B: </push-register>",
        "S: HTTP/1.0 204 No Content",
        "S: Location: https://example.com/webdav/subscriptions/io6Efei4ooph",
        "S: Expires: Sat, 01 Aug 2026 12:30:00 GMT",
        "X",
    });
    job->exec();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);
    QCOMPARE(job->registrationUrl(), QUrl::fromUserInput("https://example.com/webdav/subscriptions/io6Efei4ooph"_L1));
    QCOMPARE(job->expirationDate(), expiration);
}

QTEST_MAIN(DavPushRegistrationJobTest)

#include "moc_davpushregistrationjobtest.cpp"
