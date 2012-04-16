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
