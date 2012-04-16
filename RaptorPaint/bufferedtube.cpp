#include "bufferedtube.h"

BufferedTube::BufferedTube(QTcpSocket* socket) : QObject(), sock(socket)
{
    this->connect(this->sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

BufferedTube::~BufferedTube()
{
}

void BufferedTube::sendBuffer(QBuffer& buffer)
{
    this->m_w.lock();
    QByteArray d = buffer.buffer();
    {
        QDataStream ds(this->sock);
        ds << d.size();
    }
    this->sock->write(d);
    this->m_w.unlock();
}

void BufferedTube::readyRead()
{
    this->m_r.lock();
    static int waitingForSize = -1;

    for(;;)
    {
        if (waitingForSize == -1)
        {
            if (this->sock->bytesAvailable() > sizeof(int))
            {
                QDataStream ds(this->sock);
                ds >> waitingForSize;
            }
            else
            {
                break;
            }
        }

        if (this->sock->bytesAvailable() >= waitingForSize)
        {
            QByteArray buffer;
            buffer.resize(waitingForSize);
            this->sock->read(buffer.data(), waitingForSize);
            this->gotBuffer(buffer);

            QDataStream ds(buffer);
            QString s;
            ds >> s;
            qDebug(s.toAscii().data());

            waitingForSize = -1;
        }
        else
        {
            break;
        }
    }
    this->m_r.unlock();
}
