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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QString>
#include <QQueue>
#include <QByteArray>
#include <QThread>
#include <QMap>
#include <QImage>
#include <QMutex>

#include "glwindow.h"
#include "bufferedtube.h"

class ConnectionManager : public QObject
{
    Q_OBJECT
public:
    ConnectionManager();
    void openConnectionWindow();
    void disconnect();
    void sendTextMessage(QString message);
    bool toggleMute(QString name);
    QString getName();
    ImageStack* getLayerPtr();
    QImage* myImage();
    void sendImageUpdate();
    void forceRepaint();

private slots:
    void connectionWindowResponce(QString username, QString hostname);
    void gotDataFromBufferedTube(QByteArray buffer);

private:
    ImageStack layers;
    QMap<QString, bool> mutes;
    QMutex txtQueue;
    QQueue<QString> outboundMessages;
    QTcpSocket socket;
    BufferedTube* bt;
    bool keepAlive;
    QString name;
    QImage my_Image;
    bool sendImage;

signals:
    void gotTextMessage(QString message);
    void gotImageUpdate();
    void userJoined(QString user);
    void userLeft(QString user);

    void done();

    void repaintNow();

};

#endif // CONNECTIONMANAGER_H
