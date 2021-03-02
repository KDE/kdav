/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davitemslistjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavError>
#include <KDAV/DavItemsListJob>
#include <KDAV/DavUrl>
#include <KDAV/EtagCache>

#include <QTest>

void DavItemsListJobTest::noMatchingMimetype()
{
    std::shared_ptr<KDAV::EtagCache> cache(new KDAV::EtagCache());

    QUrl url(QStringLiteral("http://localhost/collection"));
    KDAV::DavUrl davUrl(url, KDAV::CardDav);
    KDAV::Error error(KDAV::ErrorNumber::ERR_ITEMLIST_NOMIMETYPE, 0, QString(), 0);

    auto job = new KDAV::DavItemsListJob(davUrl, cache);
    job->setContentMimeTypes(QStringList() << QStringLiteral("mime/invalid1") << QStringLiteral("mime/invalid2"));
    job->exec();

    QCOMPARE(job->error(), static_cast<int>(KDAV::ErrorNumber::ERR_ITEMLIST_NOMIMETYPE));
    QCOMPARE(job->errorText(), error.errorText());
}

QTEST_MAIN(DavItemsListJobTest)
