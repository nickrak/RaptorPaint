// Raptor Paint
// Copyright (C) 2012 Nick Rakoczy, Jessica Randall
//
// This program is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.

#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QTcpSocket>
#include <QList>
#include <QQueue>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QMutex>
#include "bufferedtube.h"

typedef QQueue<QByteArray> RaptorImageQueue;

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    SocketHandler(QTcpSocket* sock);
    ~SocketHandler();

    QString getName();

signals:
    void gotImageUpdate(QString user, QImage image);
    void gotTextMessage(QString msg);
    void destroySelf(SocketHandler* sender);

public slots:
    void sendTextMessage(QString msg);
    void sendUpdate(QString user, QImage buffer);

private slots:
    void gotDataFromSocket(QByteArray buffer);

private:
    QTcpSocket* socket;
    QMutex writeMutex;
    QMutex readMutex;
    BufferedTube bt;
    bool keepAlive;
    QString name;
    bool reading;

    QQueue<QString> pendingMessages;
    QMap<QString, RaptorImageQueue> pendingUpdates;

    void status(QDataStream& ds)
    {
        qDebug("status: %d", ds.status());
    }
};

static QList<SocketHandler*> socketHandlers;

#endif // SOCKETHANDLER_H
