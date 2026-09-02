/*
 *  SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#pragma once

#include <QObject>

class DavItemsSyncJobTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void calDavSyncChangedItems();
    void calDavSyncDeletedItems();
    void calDavSyncChangedAndDeletedItems();

    void calDavSyncMalformedMissingHref();
    void calDavSyncMalformedMissingPropstat();
};
