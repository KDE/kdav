/*
    SPDX-FileCopyrightText: 2008 Omat Holding B .V. <info@omat.nl>
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB , a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: Kevin Ottens <kevin@kdab.com>
    SPDX-FileCopyrightText: 2017 Sandro Kanuß <knauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef FAKESERVER_H
#define FAKESERVER_H

#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

Q_DECLARE_METATYPE(QList<QByteArray>)

/**
 * Pretends to be an DAV server for the purposes of unit tests.
 *
 * FakeServer does not really understand the DAV protocol.  Instead,
 * you give it a script, or scenario, that lists how an DAV session
 * exchange should go.  When it receives the client parts of the
 * scenario, it will respond with the following server parts.
 *
 * The server can be furnished with several scenarios.  The first
 * scenario will be played out to the first client that connects, the
 * second scenario to the second client connection and so on.
 *
 * The fake server runs as a separate thread in the same process it
 * is started from, and listens for connections (see port() method) on the
 * local machine.
 *
 * Scenarios are in the form of protocol messages, with a tag at the
 * start to indicate whether it is message that will be sent by the
 * client ("C: ") or a response that should be sent by the server
 * ("S: "). Or ("D: ") for the exchanged data. Content-length header is added
 * automatically with the current length and also the empty line between Header
 * and Content. For example:
 * @code
 * C: GET /item HTTP/1.1
 * S: HTTP/1.0 200 OK
 * D: much data
 * D: more data
 * X
 * @endcode
 *
 * A line starting with X indicates that the connection should be
 * closed by the server.  This should be the last line in the
 * scenario.

 * A typical usage is something like
 * @code
 * QList<QByteArray> scenario;
 * scenario << "C: GET /item HTTP/1.1"
 *          << "S: HTTP/1.0 200 OK"
 *          << "D: much data"
 *          << "D: more data"
 *          << "X";
 *
 * FakeServer fakeServer;
 * fakeServer.setScenario(scenario);
 * fakeServer.startAndWait();
 *
 * QUrl url(QStringLiteral("http://localhost/item"));
 * url.setPort(fakeServer.port());
 * KDAV::DavUrl davUrl(url, KDAV::CardDav);
 * KDAV::DavItem item(davUrl, QString(), QByteArray(), QString());
 *
 * auto job = new KDAV::DavItemFetchJob(item);
 * job->exec();
 * fakeServer.quit();
 * QVERIFY(fakeServer.isAllScenarioDone());
 * @endcode
 */

class FakeServer : public QObject
{
    Q_OBJECT

public:
    /**
     * Each unittest should use a different port so that they can be run in parallel
     */
    FakeServer(int port = 5989, QObject *parent = nullptr);
    ~FakeServer() override;

    /**
     * Starts the server and waits for it to be ready
     *
     * You should use this instead of start() to avoid race conditions.
     */
    void startAndWait();

    /**
     * Removes any previously-added scenarios, and adds a new one
     *
     * After this, there will only be one scenario, and so the fake
     * server will only be able to service a single request.  More
     * scenarios can be added with addScenario, though.
     *
     * @see addScenario()\n
     * addScenarioFromFile()
     */
    void setScenario(const QList<QByteArray> &scenario);

    /**
     * Adds a new scenario
     *
     * Note that scenarios will be used in the order that clients
     * connect.  If this is the 5th scenario that has been added
     * (bearing in mind that setScenario() resets the scenario
     * count), it will be used to service the 5th client that
     * connects.
     *
     * @see addScenarioFromFile()
     *
     * @param scenario  the scenario as a list of messages
     */
    void addScenario(const QList<QByteArray> &scenario);
    /**
     * Adds a new scenario from a local file
     *
     * Note that scenarios will be used in the order that clients
     * connect.  If this is the 5th scenario that has been added
     * (bearing in mind that setScenario() resets the scenario
     * count), it will be used to service the 5th client that
     * connects.
     *
     * @see addScenario()
     *
     * @param fileName  the name of the file that contains the
     *                  scenario; it will be split at line
     *                  boundaries, and excess whitespace will
     *                  be trimmed from the start and end of lines
     */
    void addScenarioFromFile(const QString &fileName);

    /**
     * Checks whether a particular scenario has completed
     *
     * @param scenarioNumber  the number of the scenario to check,
     *                        in order of addition/client connection
     */
    bool isScenarioDone(int scenarioNumber) const;
    /**
     * Whether all the scenarios that were added to the fake
     * server have been completed.
     */
    bool isAllScenarioDone() const;

    /**
     * Returns the port where the fake server is listening.
     */
    int port() const;

private Q_SLOTS:
    void newConnection();
    void dataAvailable();
    void init();
    void cleanup();

private:
    void writeServerPart(QTcpSocket *clientSocket, int scenarioNumber);
    void readClientPart(QTcpSocket *socket, int *scenarioNumber);

    QList<QList<QByteArray>> m_scenarios;
    QTcpServer *m_tcpServer = nullptr;
    mutable QMutex m_mutex;
    QList<QTcpSocket *> m_clientSockets;
    QThread *m_thread;
    int m_port;
};

#endif
