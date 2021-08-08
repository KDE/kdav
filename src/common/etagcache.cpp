/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "etagcache.h"

#include <QMap>
#include <QSet>

using namespace KDAV;

namespace KDAV
{
class EtagCachePrivate
{
public:
    QMap<QString, QString> mCache;
    QSet<QString> mChangedRemoteIds;
};
}

EtagCache::EtagCache(QObject *parent)
    : QObject(parent)
    , d(new EtagCachePrivate)
{
}

EtagCache::~EtagCache() = default;

void EtagCache::setEtag(const QString &remoteId, const QString &etag)
{
    setEtagInternal(remoteId, etag);

    d->mChangedRemoteIds.remove(remoteId);
}

void EtagCache::setEtagInternal(const QString &remoteId, const QString &etag)
{
    d->mCache[remoteId] = etag;
}

bool EtagCache::contains(const QString &remoteId) const
{
    return d->mCache.contains(remoteId);
}

bool EtagCache::etagChanged(const QString &remoteId, const QString &refEtag) const
{
    if (!contains(remoteId)) {
        return true;
    }
    return d->mCache.value(remoteId) != refEtag;
}

void EtagCache::markAsChanged(const QString &remoteId)
{
    d->mChangedRemoteIds.insert(remoteId);
}

bool EtagCache::isOutOfDate(const QString &remoteId) const
{
    return d->mChangedRemoteIds.contains(remoteId);
}

void EtagCache::removeEtag(const QString &remoteId)
{
    d->mChangedRemoteIds.remove(remoteId);
    d->mCache.remove(remoteId);
}

QMap<QString, QString> EtagCache::etagCacheMap() const
{
    return d->mCache;
}

QStringList EtagCache::urls() const
{
    return d->mCache.keys();
}

QStringList EtagCache::changedRemoteIds() const
{
    return d->mChangedRemoteIds.values();
}
