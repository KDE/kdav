/*
    SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "davpushsupport.h"

using namespace KDAV;

class DavPushSupportPrivate : public QSharedData
{
public:
    bool operator==(const DavPushSupportPrivate &rhs) const;

public:
    QString topic;
    QByteArray vapidPublicKey;
};

bool DavPushSupportPrivate::operator==(const DavPushSupportPrivate &rhs) const
{
    return std::tie(topic, vapidPublicKey) == std::tie(rhs.topic, rhs.vapidPublicKey);
}

DavPushSupport::DavPushSupport()
    : d(new DavPushSupportPrivate)
{
}

DavPushSupport::DavPushSupport(const DavPushSupport &other) = default;
DavPushSupport::DavPushSupport(DavPushSupport &&) = default;
DavPushSupport &DavPushSupport::operator=(const DavPushSupport &other) = default;
DavPushSupport &DavPushSupport::operator=(DavPushSupport &&) = default;
DavPushSupport::~DavPushSupport() = default;

bool DavPushSupport::operator==(const DavPushSupport &other) const
{
    return (*d) == (*other.d);
}

bool DavPushSupport::isValid() const
{
    return !d->topic.isEmpty();
}

void DavPushSupport::setTopic(const QString &topic)
{
    d->topic = topic;
}

QString DavPushSupport::topic() const
{
    return d->topic;
}

void DavPushSupport::setVapidPublicKey(const QByteArray &vapidPublicKey)
{
    d->vapidPublicKey = vapidPublicKey;
}

QByteArray DavPushSupport::vapidPublicKey() const
{
    return d->vapidPublicKey;
}
