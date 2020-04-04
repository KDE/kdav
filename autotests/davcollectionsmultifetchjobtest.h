/*
    SPDX-FileCopyrightText: 2020 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DAVITEMFETCHJOB_TEST_H
#define DAVITEMFETCHJOB_TEST_H

#include <QObject>

class DavCollectionsMultiFetchJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void runSuccessfullTest();
    void shouldFailOnError();
};

#endif
