/*
    Copyright (c) 2010 Grégory Oestreicher <greg@kamago.net>

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

#ifndef ETAGCACHE_AKONADI_H
#define ETAGCACHE_AKONADI_H

#include "etagcache.h"

namespace Akonadi
{
class Collection;
}

class KJob;

/**
 * @short A helper class to cache etags.
 *
 * The EtagCache caches the remote ids and etags of all items
 * in a given collection. This cache is needed to find
 * out which items have been changed in the backend and have to
 * be refetched on the next call of ResourceBase::retrieveItems()
 */
class EtagCacheAkonadi : public EtagCache
{
    Q_OBJECT

public:
    /**
     * Creates a new etag cache and populates it with the ETags
     * of items found in @p collection.
     */
    explicit EtagCacheAkonadi(const Akonadi::Collection &collection, QObject *parent = Q_NULLPTR);

private Q_SLOTS:
    void onItemFetchJobFinished(KJob *job);

};

#endif