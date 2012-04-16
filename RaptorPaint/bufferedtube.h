#ifndef BUFFEREDTUBE_H
#define BUFFEREDTUBE_H

#include <QTcpSocket>
#include <QDataStream>
#include <QBuffer>
#include <QImage>
#include <QMutex>
#include <QByteArray>

class BufferedTube : public QObject
{
    Q_OBJECT
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
    QMutex m_r;
    QMutex m_w;
};

#endif // BUFFEREDTUBE_H
