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
#include <QTcpServer>
#include <QFile>
#include <QTest>

FakeServer::FakeServer(QObject *parent)
    : QThread(parent)
    , m_port(5989)
{
    moveToThread(this);
}

FakeServer::~FakeServer()
{
    quit();
    wait();
}

void FakeServer::startAndWait()
{
    start();
    // this will block until the event queue starts
    QMetaObject::invokeMethod(this, &FakeServer::started, Qt::BlockingQueuedConnection);
}

void FakeServer::dataAvailable()
{
    QMutexLocker locker(&m_mutex);

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket != nullptr);

    int scenarioNumber = m_clientSockets.indexOf(socket);

    if (scenarioNumber >= m_scenarios.size()) {
        qWarning() << "There is no scenario for socket" << scenarioNumber << ", we got more connections than expected";
    }

    readClientPart(scenarioNumber);
    writeServerPart(scenarioNumber);
}

void FakeServer::newConnection()
{
    QMutexLocker locker(&m_mutex);

    m_clientSockets << m_tcpServer->nextPendingConnection();
    connect(m_clientSockets.last(), &QTcpSocket::readyRead, this, &FakeServer::dataAvailable);
}

void FakeServer::run()
{
    m_tcpServer = new QTcpServer();

    if (!m_tcpServer->listen(QHostAddress(QHostAddress::LocalHost), m_port)) {
        qFatal("Unable to start the server");
    }

    connect(m_tcpServer, &QTcpServer::newConnection, this, &FakeServer::newConnection);

    exec();

    qDeleteAll(m_clientSockets);

    delete m_tcpServer;
}

void FakeServer::started()
{
    // do nothing: this is a dummy slot used by startAndWait()
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

    for (const QList<QByteArray> &scenario : qAsConst(m_scenarios)) {
        if (!scenario.isEmpty()) {
            return false;
        }
    }

    return true;
}

void FakeServer::writeServerPart(int scenarioNumber)
{
    QList<QByteArray> scenario = m_scenarios[scenarioNumber];
    QTcpSocket *clientSocket = m_clientSockets[scenarioNumber];

    while (!scenario.isEmpty() &&
            scenario.first().startsWith("S: ")) {
        QByteArray rule = scenario.takeFirst();

        QByteArray payload = rule.mid(3);
        clientSocket->write(payload + "\r\n");
    }

    QByteArray data;

    while (!scenario.isEmpty() &&
            scenario.first().startsWith("D: ")) {
        QByteArray rule = scenario.takeFirst();

        QByteArray payload = rule.mid(3);
        data.append(payload + "\r\n");
    }

    clientSocket->write(QStringLiteral("Content-Length: %1\r\n\r\n").arg(data.length()).toLatin1());
    clientSocket->write(data);

    if (!scenario.isEmpty() &&
            scenario.first().startsWith("X")) {
        scenario.takeFirst();
        clientSocket->close();
    }

    if (!scenario.isEmpty()) {
        QVERIFY(scenario.first().startsWith("C: "));
    }

    m_scenarios[scenarioNumber] = scenario;
}

void FakeServer::readClientPart(int scenarioNumber)
{
    QList<QByteArray> scenario = m_scenarios[scenarioNumber];
    QTcpSocket *socket = m_clientSockets[scenarioNumber];
    QByteArray line = socket->readLine();
    qDebug() << "Read client request" << line;
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

    while (!scenario.isEmpty() &&
            scenario.first().startsWith("C: ")) {
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

    m_scenarios[scenarioNumber] = scenario;
}

int FakeServer::port() const
{
    return m_port;
}
