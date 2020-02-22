/*
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DAVITEMSLISTJOB_TEST_H
#define DAVITEMSLISTJOB_TEST_H

#include <QObject>

class DavItemsListJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void noMatchingMimetype();
};

#endif
