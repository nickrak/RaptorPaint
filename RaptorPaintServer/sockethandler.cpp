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

#include "sockethandler.h"
#include <QDebug>
#include <QBuffer>

// Newly received connection
SocketHandler::SocketHandler(QTcpSocket* sock) :
    QObject(), bt(sock), reading(false)
{
    this->socket = sock;
    socketHandlers << this;
    foreach (SocketHandler* handler, socketHandlers)
    {
        this->connect(handler, SIGNAL(gotImageUpdate(QString,QImage)), this, SLOT(sendUpdate(QString,QImage)));
        this->connect(handler, SIGNAL(gotTextMessage(QString)), this, SLOT(sendTextMessage(QString)));

        if (handler != this)
        {
            this->connect(this, SIGNAL(gotImageUpdate(QString,QImage)), handler, SLOT(sendUpdate(QString,QImage)));
            this->connect(this, SIGNAL(gotTextMessage(QString)), handler, SLOT(sendTextMessage(QString)));
        }
    }

    this->connect(&this->bt, SIGNAL(gotBuffer(QByteArray)), this, SLOT(gotDataFromSocket(QByteArray)));
    this->connect(this->socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
}

// Connection Lost
SocketHandler::~SocketHandler()
{
    this->gotTextMessage(QString("[**SERVER**] %1 left.").arg(this->name));

    qDebug("SocketHandler erase");

    socketHandlers.removeAll(this);

    if (this->socket->isOpen())
    {
        this->socket->flush();
        this->socket->close();
    }

    this->socket->deleteLater();
}

QString SocketHandler::getName()
{
    return this->name;
}

void SocketHandler::gotDataFromSocket(QByteArray buffer)
{
    QDataStream ds(buffer);
    QString type;
    ds >> type;

    if (type == "MSG")
    {
        qDebug("Got Message from %s", this->name.toAscii().data());
        QString message;
        ds >> message;
        this->gotTextMessage(message.prepend("[%1] ").arg(this->name));
    }
    else if (type == "UPD")
    {
        qDebug("Got Update from %s", this->name.toAscii().data());
        QImage img;
        ds >> img;
        this->gotImageUpdate(this->name, img);
    }
    else if (type == "ID")
    {
        qDebug("Got ID");
        ds >> this->name;
        this->gotTextMessage(QString("[**SERVER**] %1 joined.").arg(this->name));
    }
    else
    {
        this->status(ds);
        qDebug(type.prepend("BIG FUCKING ERROR: ").append("^").append(type.toAscii().toHex()).append("$")
               .append(QString::number(this->socket->bytesAvailable())).toAscii().data());
        exit(1);
    }
}

// Some other client sent a text message, please forward to this client
void SocketHandler::sendTextMessage(QString msg)
{
    this->writeMutex.lock();
    QBuffer b;
    b.open(QIODevice::WriteOnly);
    {
        QDataStream ds(&b);
        ds << QString("TXT") << msg;
    }
    this->bt.sendBuffer(b);
    qDebug("send");
    this->writeMutex.unlock();
}

// Some other client updated image, please forward to this client
void SocketHandler::sendUpdate(QString user, QImage buffer)
{
    if (user != this->name)
    {
        this->writeMutex.lock();
        QBuffer b;
        b.open(QIODevice::WriteOnly);
        {
            QDataStream ds (&b);
            ds << QString("IMG") << user << buffer;
        }
        this->bt.sendBuffer(b);
        this->writeMutex.unlock();
    }
}
