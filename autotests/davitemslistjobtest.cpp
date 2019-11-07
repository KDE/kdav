/*
    Copyright (c) 2017 Sandro Knauß <sknauss@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "davitemslistjobtest.h"
#include "fakeserver.h"

#include <KDAV/DavItemsListJob>
#include <KDAV/DavUrl>
#include <KDAV/EtagCache>
#include <KDAV/DavError>

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
