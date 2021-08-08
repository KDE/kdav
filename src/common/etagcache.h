/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_ETAGCACHE_H
#define KDAV_ETAGCACHE_H

#include "kdav_export.h"

#include <QObject>
#include <QStringList>

#include <memory>

namespace KDAV
{
class EtagCachePrivate;

/**
 * @class EtagCache etagcache.h <KDAV/EtagCache>
 *
 * @short A helper class to cache ETags.
 *
 * The EtagCache caches the remote ids and ETags of all items
 * in a given collection. This cache is needed to find
 * out which items have been changed in the backend and have to
 * be refetched on the next call of Akonadi::ResourceBase::retrieveItems()
 */
class KDAV_EXPORT EtagCache : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new ETag cache and populates it with the ETags
     * of items found in @p collection.
     */
    explicit EtagCache(QObject *parent = nullptr);
    ~EtagCache();

    /**
     * Sets the ETag for the remote ID. If the remote ID is marked as
     * changed (is contained in the return of changedRemoteIds), remove
     * it from the changed list.
     */
    void setEtag(const QString &remoteId, const QString &etag);

    /**
     * Checks if the given item is in the cache
     */
    Q_REQUIRED_RESULT bool contains(const QString &remoteId) const;

    /**
     * Check if the known ETag for the remote ID is equal to @p refEtag.
     */
    Q_REQUIRED_RESULT bool etagChanged(const QString &remoteId, const QString &refEtag) const;

    /**
     * Mark an item as changed in the backend.
     */
    void markAsChanged(const QString &remoteId);

    /**
     * Returns true if the remote ID is marked as changed (is contained in the
     * return of changedRemoteIds)
     */
    Q_REQUIRED_RESULT bool isOutOfDate(const QString &remoteId) const;

    /**
     * Removes the entry for item with remote ID @p remoteId.
     */
    void removeEtag(const QString &remoteId);

    /**
     * Returns the list of all items URLs.
     */
    Q_REQUIRED_RESULT QStringList urls() const;

    /**
     * Returns the list of remote ids of items that have been changed
     * in the backend.
     */
    Q_REQUIRED_RESULT QStringList changedRemoteIds() const;

protected:
    /**
     * Sets the ETag for the remote ID.
     */
    void setEtagInternal(const QString &remoteId, const QString &etag);

private:
    const std::unique_ptr<EtagCachePrivate> d;

    friend class DavItemsListJobPrivate;
    // @internal
    // Returns a map of remote Id and corresponding etag string key/value pairs.
    // Only used by DavItemsListJobPrivate
    Q_DECL_HIDDEN QMap<QString, QString> etagCacheMap() const;
};
}

#endif
