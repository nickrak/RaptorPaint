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

#include "bufferedtube.h"

BufferedTube::BufferedTube(QTcpSocket* socket) : QObject(), sock(socket)
{
    this->connect(this->sock, SIGNAL(readyRead()), this, SLOT(readyRead()));
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
