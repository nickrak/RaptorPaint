#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QList>
#include <QQueue>
#include <QMap>
#include <QList>
#include <QByteArray>

typedef QQueue<QByteArray> QByteArrayQueue;

class SocketHandler : private QThread
{
    Q_OBJECT
public:
    SocketHandler(QTcpSocket* sock);
    ~SocketHandler();
    
signals:
    void gotImageUpdate(QString user, QByteArray buffer);
    void gotTextMessage(QString msg);
    void destroySelf(SocketHandler* sender);

public slots:
    void sendTextMessage(QString msg);
    void sendUpdate(QString user, QByteArray buffer);

private:
    QTcpSocket* socket;
    QDataStream ds;
    bool keepAlive;
    QString name;

    QQueue<QString> pendingMessages;
    QMap<QString, QByteArrayQueue> pendingUpdates;

    void run();
};

static QList<SocketHandler*> socketHandlers;

#endif // SOCKETHANDLER_H
