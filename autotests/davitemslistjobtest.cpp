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

#include "davitemslistjobtest.h"
#include "fakeserver.h"

#include <KDAV2/DavItemsListJob>
#include <KDAV2/DavUrl>
#include <KDAV2/EtagCache>

#include <QTest>

void DavItemsListJobTest::noMatchingMimetype()
{
    std::shared_ptr<KDAV2::EtagCache> cache(new KDAV2::EtagCache());

    QUrl url(QStringLiteral("http://localhost/collection"));
    KDAV2::DavUrl davUrl(url, KDAV2::CardDav);

    auto job = new KDAV2::DavItemsListJob(davUrl, cache);
    job->setContentMimeTypes(QStringList() << QStringLiteral("mime/invalid1") << QStringLiteral("mime/invalid2"));
    job->exec();

    QCOMPARE(job->error(), 431);
    QCOMPARE(job->errorText(), QStringLiteral("There was a problem with the request. The requested mimetypes are not supported."));

}

QTEST_GUILESS_MAIN(DavItemsListJobTest)
