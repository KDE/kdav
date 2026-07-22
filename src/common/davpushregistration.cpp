// SPDX-FileCopyrightText: 2026 Dominique Michel <dominique.michel@enioka.com>
//
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "davpushregistration.h"

#include <QDateTime>
#include <QUrl>

using namespace KDAV;
using namespace Qt::StringLiterals;

namespace KDAV
{

class DavPushRegistrationPrivate : public QSharedData
{
public:
    QUrl mPushEndpoint;
    QByteArray mSubscriptionPublicKey;
    QByteArray mAuthToken;
    QDateTime mExpiration;
};
}

DavPushRegistration::DavPushRegistration()
    : d(new DavPushRegistrationPrivate)
{
}

DavPushRegistration::DavPushRegistration(const DavPushRegistration &other) = default;
DavPushRegistration &DavPushRegistration::operator=(const DavPushRegistration &other) = default;
DavPushRegistration::DavPushRegistration(DavPushRegistration &&) noexcept = default;
DavPushRegistration &DavPushRegistration::operator=(DavPushRegistration &&) noexcept = default;
DavPushRegistration::~DavPushRegistration() = default;

DavPushRegistration::DavPushRegistration(const QUrl &pushEndpoint,
                                         const QByteArray &subscriptionPublicKey,
                                         const QByteArray &authToken,
                                         const QDateTime &expiration)
    : DavPushRegistration()
{
    d->mPushEndpoint = pushEndpoint;
    d->mSubscriptionPublicKey = subscriptionPublicKey;
    d->mAuthToken = authToken;
    d->mExpiration = expiration;
}

void DavPushRegistration::setPushEndpoint(const QUrl &pushEndpoint)
{
    d->mPushEndpoint = pushEndpoint;
}

QUrl DavPushRegistration::pushEndpoint() const
{
    return d->mPushEndpoint;
}

void DavPushRegistration::setSubscriptionPublicKey(const QByteArray &subscriptionPublicKey)
{
    d->mSubscriptionPublicKey = subscriptionPublicKey;
}

QByteArray DavPushRegistration::subscriptionPublicKey() const
{
    return d->mSubscriptionPublicKey;
}

void DavPushRegistration::setAuthToken(const QByteArray &authToken)
{
    d->mAuthToken = authToken;
}

QByteArray DavPushRegistration::authToken() const
{
    return d->mAuthToken;
}

void DavPushRegistration::setExpiration(const QDateTime &expiration)
{
    d->mExpiration = expiration;
}

QDateTime DavPushRegistration::expiration() const
{
    return d->mExpiration;
}

#include "moc_davpushregistration.cpp"
