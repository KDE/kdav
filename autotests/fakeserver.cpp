/*
    SPDX-FileCopyrightText: 2008 Omat Holding B .V. <info@omat.nl>
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB , a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: Kevin Ottens <kevin@kdab.com>
    SPDX-FileCopyrightText: 2017 Sandro Kanuß <knauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

// Own
#include "fakeserver.h"

// Qt
#include <QDebug>
#include <QFile>
#include <QTest>

FakeServer::FakeServer(int port, QObject *parent)
    : QObject(parent)
    , m_thread(new QThread)
    , m_port(port)
{
    moveToThread(m_thread);
}

FakeServer::~FakeServer()
{
    QMetaObject::invokeMethod(this, &FakeServer::cleanup);
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
}

void FakeServer::startAndWait()
{
    m_thread->start();
    // this will block until the init code happens and the event queue starts
    QMetaObject::invokeMethod(this, &FakeServer::init, Qt::BlockingQueuedConnection);
}

void FakeServer::dataAvailable()
{
    QMutexLocker locker(&m_mutex);

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket != nullptr);

    int scenarioNumber = -1;
    readClientPart(socket, &scenarioNumber);
    if (scenarioNumber >= 0) {
        Q_ASSERT(scenarioNumber < m_scenarios.count());
        writeServerPart(socket, scenarioNumber);
    }
}

void FakeServer::newConnection()
{
    QMutexLocker locker(&m_mutex);

    m_clientSockets << m_tcpServer->nextPendingConnection();
    connect(m_clientSockets.last(), &QTcpSocket::readyRead, this, &FakeServer::dataAvailable);
}

void FakeServer::init()
{
    m_tcpServer = new QTcpServer();

    if (!m_tcpServer->listen(QHostAddress(QHostAddress::LocalHost), m_port)) {
        qFatal("Unable to start the server");
    }

    connect(m_tcpServer, &QTcpServer::newConnection, this, &FakeServer::newConnection);
}

void FakeServer::cleanup()
{
    qDeleteAll(m_clientSockets);
    delete m_tcpServer;
}

void FakeServer::setScenario(const QList<QByteArray> &scenario)
{
    QMutexLocker locker(&m_mutex);

    m_scenarios.clear();
    m_scenarios << scenario;
}

void FakeServer::addScenario(const QList<QByteArray> &scenario)
{
    QMutexLocker locker(&m_mutex);

    m_scenarios << scenario;
}

void FakeServer::addScenarioFromFile(const QString &fileName)
{
    QFile file(fileName);
    file.open(QFile::ReadOnly);

    QList<QByteArray> scenario;

    while (!file.atEnd()) {
        scenario << file.readLine().trimmed();
    }

    file.close();

    addScenario(scenario);
}

bool FakeServer::isScenarioDone(int scenarioNumber) const
{
    QMutexLocker locker(&m_mutex);

    if (scenarioNumber < m_scenarios.size()) {
        return m_scenarios[scenarioNumber].isEmpty();
    } else {
        return true; // Non existent hence empty, right?
    }
}

bool FakeServer::isAllScenarioDone() const
{
    QMutexLocker locker(&m_mutex);

    for (const QList<QByteArray> &scenario : std::as_const(m_scenarios)) {
        if (!scenario.isEmpty()) {
            return false;
        }
    }

    return true;
}

void FakeServer::writeServerPart(QTcpSocket *clientSocket, int scenarioNumber)
{
    QList<QByteArray> scenario = m_scenarios[scenarioNumber];

    while (!scenario.isEmpty() && scenario.first().startsWith("S: ")) {
        QByteArray rule = scenario.takeFirst();

        QByteArray payload = rule.mid(3);
        clientSocket->write(payload + "\r\n");
    }

    QByteArray data;

    while (!scenario.isEmpty() && scenario.first().startsWith("D: ")) {
        QByteArray rule = scenario.takeFirst();

        QByteArray payload = rule.mid(3);
        data.append(payload + "\r\n");
    }

    clientSocket->write(QStringLiteral("Content-Length: %1\r\n\r\n").arg(data.length()).toLatin1());
    clientSocket->write(data);

    if (!scenario.isEmpty() && scenario.first().startsWith("X")) {
        scenario.takeFirst();
        clientSocket->close();
    }

    if (!scenario.isEmpty()) {
        QVERIFY(scenario.first().startsWith("C: "));
    }

    m_scenarios[scenarioNumber] = scenario;
}

void FakeServer::readClientPart(QTcpSocket *socket, int *scenarioNumber)
{
    QByteArray line = socket->readLine();
    qDebug() << "Read client request" << line;
    const auto it = std::find_if(m_scenarios.begin(), m_scenarios.end(), [&](const QList<QByteArray> &scenario) {
        return !scenario.isEmpty() && scenario.at(0).mid(3) + "\r\n" == line;
    });
    if (it == m_scenarios.end()) {
        qWarning() << "No server response available for" << line;
        QFAIL("Unexpected request");
        return;
    }
    QList<QByteArray> scenario = *it;
    QVector<QByteArray> header;

    while (line != "\r\n") {
        header << line;
        if (socket->bytesAvailable() == 0 && !socket->waitForReadyRead()) {
            qDebug() << header;
            QFAIL("could not read all headers");
            return;
        }
        line = socket->readLine();
    }

    while (!scenario.isEmpty() && scenario.first().startsWith("C: ")) {
        QByteArray expected = scenario.takeFirst().mid(3) + "\r\n";

        if (!header.contains(expected)) {
            qWarning() << expected << "not found in header. Here's what we got:";
            qWarning() << header;
            QVERIFY(false);
            break;
        }
    }

    if (!scenario.isEmpty()) {
        QVERIFY(scenario.first().startsWith("S: "));
    }

    *it = scenario;
    *scenarioNumber = std::distance(m_scenarios.begin(), it);
    QVERIFY(*scenarioNumber < m_scenarios.count());
}

int FakeServer::port() const
{
    return m_port;
}
