/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davpushdontnotify.h"

#include <QList>

using namespace KDAV;
using namespace Qt::Literals;

class DavPushDontNotifyPrivate : public QSharedData
{
public:
    QStringList mIgnoredUrls;
    bool mIgnoreAll = false;
};

DavPushDontNotify::DavPushDontNotify()
    : d(new DavPushDontNotifyPrivate)
{
}

DavPushDontNotify::DavPushDontNotify(const DavPushDontNotify &other) = default;
DavPushDontNotify::DavPushDontNotify(DavPushDontNotify &&) noexcept = default;
DavPushDontNotify &DavPushDontNotify::operator=(const DavPushDontNotify &other) = default;
DavPushDontNotify &DavPushDontNotify::operator=(DavPushDontNotify &&) noexcept = default;
DavPushDontNotify::~DavPushDontNotify() = default;

DavPushDontNotify DavPushDontNotify::ignoreAll()
{
    auto ret = DavPushDontNotify();
    ret.setIgnoreAll(true);
    return ret;
}

DavPushDontNotify DavPushDontNotify::ignoreUrls(const QStringList &urls)
{
    auto ret = DavPushDontNotify();
    ret.setIgnoredUrls(urls);
    return ret;
}

bool DavPushDontNotify::isNull() const
{
    return d->mIgnoredUrls.isEmpty() && !d->mIgnoreAll;
}

void DavPushDontNotify::setIgnoredUrls(const QStringList &ignoredUrls)
{
    Q_ASSERT(!d->mIgnoredUrls.contains(QLatin1StringView("*")));

    d->mIgnoreAll = false;
    d->mIgnoredUrls = ignoredUrls;
}

QStringList DavPushDontNotify::ignoredUrls() const
{
    return d->mIgnoredUrls;
}

void DavPushDontNotify::setIgnoreAll(bool ignoreAll)
{
    d->mIgnoreAll = ignoreAll;
    d->mIgnoredUrls.clear();
}

bool DavPushDontNotify::isIgnoreAll() const
{
    return d->mIgnoreAll;
}

QString DavPushDontNotify::davHeaderName()
{
    return QLatin1StringView("DavPush-DontNotify");
}

QString DavPushDontNotify::davHeaderValue() const
{
    auto res = QString();
    if (d->mIgnoreAll) {
        res += QLatin1StringView("*");
    } else {
        for (auto it = d->mIgnoredUrls.constBegin(); it != d->mIgnoredUrls.constEnd(); ++it) {
            if (it != d->mIgnoredUrls.constBegin()) {
                res += QLatin1StringView(", ");
            }
            res += QLatin1StringView("\"") + *it + QLatin1StringView("\"");
        }
    }
    return res;
}

QString DavPushDontNotify::davHeader() const
{
    if (isNull()) {
        return QString();
    }
    return u"%1: %2"_s.arg(davHeaderName(), davHeaderValue());
}
