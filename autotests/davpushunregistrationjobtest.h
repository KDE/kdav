// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QObject>

class DavPushRegistrationJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    // Success case
    void CalDavPushUnregistration_normal();
    void CalDavPushUnregistration_missing();

    // Error case
    void CalDavPushUnregistration_fail();
};
