#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QTcpSocket>
#include <QList>
#include <QQueue>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QMutex>

typedef QQueue<QByteArray> QByteArrayQueue;

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    SocketHandler(QTcpSocket* sock);
    ~SocketHandler();

    QString getName();
    
signals:
    void gotImageUpdate(QString user, QByteArray buffer);
    void gotTextMessage(QString msg);
    void destroySelf(SocketHandler* sender);

public slots:
    void sendTextMessage(QString msg);
    void sendUpdate(QString user, QByteArray buffer);

private slots:
    void gotDataFromSocket();

private:
    QTcpSocket* socket;
    QMutex m;
    QDataStream ds;
    bool keepAlive;
    QString name;
    bool reading;

    QQueue<QString> pendingMessages;
    QMap<QString, QByteArrayQueue> pendingUpdates;
};

static QList<SocketHandler*> socketHandlers;

#endif // SOCKETHANDLER_H
