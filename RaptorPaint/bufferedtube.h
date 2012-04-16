#ifndef BUFFEREDTUBE_H
#define BUFFEREDTUBE_H

#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <QImage>

class BufferedTube
{
public:
    BufferedTube(QTcpSocket* socket);
    ~BufferedTube();

    void sendBuffer(QBuffer &buffer);

private slots:
    void readyRead();

signals:
    void gotBuffer(QByteArray buffer);

private:
    QTcpSocket* sock;
    QDataStream ds;
};

#endif // BUFFEREDTUBE_H
