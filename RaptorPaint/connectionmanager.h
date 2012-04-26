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
