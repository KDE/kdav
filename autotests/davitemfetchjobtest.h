/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DAVITEMFETCHJOB_TEST_H
#define DAVITEMFETCHJOB_TEST_H

#include <QObject>

class DavItemFetchJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void runSuccessfullTest();
};

#endif
