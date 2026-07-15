/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DAVPUSHDONTNOTIFY_TEST_H
#define DAVPUSHDONTNOTIFY_TEST_H

#include <QObject>

class DavPushDontNotifyTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void createEmpty();
    void ignoreAll();
    void ignoreUrls();
};

#endif
