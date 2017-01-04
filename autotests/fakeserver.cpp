/*
   Copyright (C) 2008 Omat Holding B.V. <info@omat.nl>

   Copyright (C) 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
   Author: Kevin Ottens <kevin@kdab.com>

   Copyright (C) 2017 Sandro Kanuß <knauss@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

// Own
#include "fakeserver.h"

// Qt
#include <QDebug>
#include <QTcpServer>

#include <QDebug>
#include <QFile>
#include <qtest.h>

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
    QMetaObject::invokeMethod(this, "started", Qt::BlockingQueuedConnection);
}

void FakeServer::dataAvailable()
{
    QMutexLocker locker(&m_mutex);

    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    Q_ASSERT(socket != 0);

    int scenarioNumber = m_clientSockets.indexOf(socket);

    readClientPart(scenarioNumber);
    writeServerPart(scenarioNumber);
}

void FakeServer::newConnection()
{
    QMutexLocker locker(&m_mutex);

    m_clientSockets << m_tcpServer->nextPendingConnection();
    connect(m_clientSockets.last(), SIGNAL(readyRead()), this, SLOT(dataAvailable()));
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

    foreach (const QList<QByteArray> &scenario, m_scenarios) {
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
    QVector<QByteArray> header;

    while(line != "\r\n") {
        header << line;
        line = socket->readLine();
    }

    while (!scenario.isEmpty() &&
            scenario.first().startsWith("C: ")) {
        QByteArray expected = scenario.takeFirst().mid(3) + "\r\n";

        if (!header.contains(expected)) {
            qWarning() << expected << "not found in header";
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
