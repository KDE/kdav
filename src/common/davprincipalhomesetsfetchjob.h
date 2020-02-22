/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPRINCIPALHOMESETSFETCHJOB_H
#define KDAV_DAVPRINCIPALHOMESETSFETCHJOB_H

#include "kdav_export.h"

#include "davjobbase.h"
#include "davurl.h"

#include <KJob>

#include <QStringList>

namespace KDAV {
class DavPrincipalHomeSetsFetchJobPrivate;

/**
 * @short A job that fetches home sets for a principal.
 */
class KDAV_EXPORT DavPrincipalHomeSetsFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new dav principals home sets fetch job.
     *
     * @param url The DAV url of the DAV principal.
     * @param parent The parent object.
     */
    explicit DavPrincipalHomeSetsFetchJob(const DavUrl &url, QObject *parent = nullptr);

    /**
     * Starts the job.
     */
    void start() override;

    /**
     * Returns the found home sets.
     */
    Q_REQUIRED_RESULT QStringList homeSets() const;

private:
    void davJobFinished(KJob *);
    /**
     * Start the fetch process.
     *
     * There may be two rounds necessary if the first request
     * does not returns the home sets, but only the current-user-principal
     * or the principal-URL. The bool flag is here to prevent requesting
     * those last two on each request, as they are only fetched in
     * the first round.
     *
     * @param fetchHomeSetsOnly If set to true the request will not include
     *        the current-user-principal and principal-URL props.
     */
    void fetchHomeSets(bool fetchHomeSetsOnly);

    Q_DECLARE_PRIVATE(DavPrincipalHomeSetsFetchJob)
};
}

#endif
