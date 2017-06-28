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

#include "etagcache.h"

using namespace KDAV2;

EtagCache::EtagCache(QObject *parent)
    : QObject(parent)
{

}

void EtagCache::setEtag(const QString &remoteId, const QString &etag)
{
    setEtagInternal(remoteId, etag);

    if (mChangedRemoteIds.contains(remoteId)) {
        mChangedRemoteIds.remove(remoteId);
    }
}

void EtagCache::setEtagInternal(const QString &remoteId, const QString &etag)
{
    mCache[ remoteId ] = etag;
}

bool EtagCache::contains(const QString &remoteId) const
{
    return mCache.contains(remoteId);
}

bool EtagCache::etagChanged(const QString &remoteId, const QString &refEtag) const
{
    if (!contains(remoteId)) {
        return true;
    }
    return mCache.value(remoteId) != refEtag;
}

void EtagCache::markAsChanged(const QString &remoteId)
{
    mChangedRemoteIds.insert(remoteId);
}

bool EtagCache::isOutOfDate(const QString &remoteId) const
{
    return mChangedRemoteIds.contains(remoteId);
}

void EtagCache::removeEtag(const QString &remoteId)
{
    mChangedRemoteIds.remove(remoteId);
    mCache.remove(remoteId);
}

QStringList EtagCache::urls() const
{
    return mCache.keys();
}

QStringList EtagCache::changedRemoteIds() const
{
    return mChangedRemoteIds.toList();
}

