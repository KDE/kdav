/*
    SPDX-FileCopyrightText: 2010 Grégory Oestreicher <greg@kamago.net>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KDAV_DAVPRINCIPALHOMESETSFETCHJOB_H
#define KDAV_DAVPRINCIPALHOMESETSFETCHJOB_H

#include "kdav_export.h"

#include "davjobbase.h"
#include "davurl.h"

#include <QStringList>

namespace KDAV
{
class DavPrincipalHomeSetsFetchJobPrivate;

/**
 * @class DavPrincipalHomeSetsFetchJob davprincipalhomesetsfetchjob.h <KDAV/DavPrincipalHomeSetsFetchJob>
 *
 * @short A job that fetches home sets for a principal.
 */
class KDAV_EXPORT DavPrincipalHomeSetsFetchJob : public DavJobBase
{
    Q_OBJECT

public:
    /**
     * Creates a new DAV principals home sets fetch job.
     *
     * @param url The DAV URL of the DAV principal.
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
    Q_DECLARE_PRIVATE(DavPrincipalHomeSetsFetchJob)
};
}

#endif
