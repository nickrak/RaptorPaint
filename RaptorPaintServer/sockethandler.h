#ifndef SOCKETHANDLER_H
#define SOCKETHANDLER_H

#include <QTcpSocket>
#include <QList>
#include <QQueue>
#include <QMap>
#include <QList>
#include <QByteArray>
#include <QMutex>
#include <QByteArray>

typedef QQueue<QByteArray> RaptorImageQueue;

class SocketHandler : public QObject
{
    Q_OBJECT
public:
    SocketHandler(QTcpSocket* sock);
    ~SocketHandler();

    QString getName();
    
signals:
    void gotImageUpdate(QString user, QByteArray image);
    void gotTextMessage(QString msg);
    void destroySelf(SocketHandler* sender);

public slots:
    void sendTextMessage(QString msg);
    void sendUpdate(QString user, QByteArray buffer);

private slots:
    void gotDataFromSocket();

private:
    QTcpSocket* socket;
    QMutex writeMutex;
    QMutex readMutex;
    QDataStream ds;
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
