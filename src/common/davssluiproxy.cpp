/*
    SPDX-FileCopyrightText: 2026 Kevin Ottens <kevin.ottens@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davssluiproxy.h"

#include "davmanager_p.h"

void KDAV::DavSslUiProxy::setDefaultProxy(std::unique_ptr<DavSslUiProxy> &&proxy)
{
    KDAV::DavManager::self()->setSslUiProxy(std::move(proxy));
}

KDAV::DavSslUiProxy::~DavSslUiProxy() = default;
