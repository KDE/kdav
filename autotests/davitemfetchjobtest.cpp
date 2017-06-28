/*
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "davitemfetchjobtest.h"
#include "fakeserver.h"

#include <KDAV2/DavItemFetchJob>

#include <QTest>

void DavItemFetchJobTest::runSuccessfullTest()
{
    FakeServer fakeServer;
    QUrl url(QStringLiteral("http://localhost/item"));
    url.setPort(fakeServer.port());
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);

    KDAV2::DavItem item(davUrl, QString(), QByteArray(), QString());

    auto job = new KDAV2::DavItemFetchJob(item);

    fakeServer.addScenarioFromFile(QLatin1String(AUTOTEST_DATA_DIR)+QStringLiteral("/dataitemfetchjob.txt"));
    fakeServer.startAndWait();
    job->exec();
    fakeServer.quit();

    QVERIFY(fakeServer.isAllScenarioDone());
    QCOMPARE(job->error(), 0);

    QCOMPARE(item.data(), QByteArray());
    QCOMPARE(item.etag(), QString());
    QCOMPARE(item.contentType(), QString());

    item = job->item();
    QByteArray data("BEGIN:VCARD\r\nVERSION:3.0\r\nPRODID:-//Kolab//iRony DAV Server 0.3.1//Sabre//Sabre VObject 2.1.7//EN\r\nUID:12345678-1234-1234-1234-123456789abc\r\nFN:John2 Doe\r\nN:Doe;John2;;;\r\nEMAIL;TYPE=INTERNET;TYPE=HOME:john2.doe@example.com\r\nREV;VALUE=DATE-TIME:20170104T182647Z\r\nEND:VCARD\r\n");
    QCOMPARE(item.data(), data);
    QCOMPARE(item.etag(), QStringLiteral("7a33141f192d904d-47"));
    QCOMPARE(item.contentType(), QStringLiteral("text/x-vcard"));

}

QTEST_GUILESS_MAIN(DavItemFetchJobTest)
