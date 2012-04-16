#include "bufferedtube.h"

BufferedTube::BufferedTube(QTcpSocket* socket) : sock(socket)
{
    this->connect(this->sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

BufferedTube::~BufferedTube()
{
}

void BufferedTube::sendBuffer(QBuffer& buffer)
{
    QByteArray d = buffer.buffer();
    {
        QDataStream ds(this->sock);
        ds << d.size();
    }
    this->sock->write(d);
}

void BufferedTube::readyRead()
{
    static int waitingForSize = -1;

    for(;;)
    {
        if (waitingForSize == -1)
        {
            if (this->sock->bytesAvailable() > sizeof(int))
            {
                QDataStream ds(this->sock);
                ds << waitingForSize;
            }
            else
            {
                return;
            }
        }

        if (this->sock->bytesAvailable() > waitingForSize)
        {
            QByteArray buffer;
            buffer.resize(waitingForSize);
            this->sock->read(buffer.data(), buffer.size());

            this->gotBuffer(buffer);

            waitingForSize = -1;
        }
        else
        {
            return;
        }
    }
}
