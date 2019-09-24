/*
    Copyright (c) 2019 Volker Krause <vkrause@kde.org>

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

#ifndef KDAV_DAVMANAGER_P_H
#define KDAV_DAVMANAGER_P_H

#include "davmanager.h"
#include "enums.h"

#include <memory>

namespace KDAV {
class DavProtocolBase;

class DavManagerPrivate
{
public:
    /**
     * Returns the DAV protocol dialect object for the given DAV @p protocol.
     */
    static const DavProtocolBase *davProtocol(Protocol protocol);

    static inline DavManagerPrivate* get(DavManager *mgr)
    {
        return mgr->d.get();
    }

    std::unique_ptr<DavProtocolBase> mProtocols[PROTOCOL_COUNT];
};

}

#endif

