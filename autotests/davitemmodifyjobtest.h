// SPDX-FileCopyrightText: 2026 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavItemModifyJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();
    void runSuccessfulTest();
    void runSuccessWithLocationRedirect();
    void runServerErrorTest();
    void runConflictTest();
};
