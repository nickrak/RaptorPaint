#include "sockethandler.h"
#include <QDebug>

// Newly received connection
SocketHandler::SocketHandler(QTcpSocket* sock) :
    QObject(), ds(sock), reading(false)
{
    this->socket = sock;
    socketHandlers << this;
    foreach (SocketHandler* handler, socketHandlers)
    {
        this->connect(handler, SIGNAL(gotImageUpdate(QString,QByteArray)), this, SLOT(sendUpdate(QString,QByteArray)));
        this->connect(handler, SIGNAL(gotTextMessage(QString)), this, SLOT(sendTextMessage(QString)));

        if (handler != this)
        {
            this->connect(this, SIGNAL(gotImageUpdate(QString,QByteArray)), handler, SLOT(sendUpdate(QString,QByteArray)));
            this->connect(this, SIGNAL(gotTextMessage(QString)), handler, SLOT(sendTextMessage(QString)));
        }
    }

    this->connect(this->socket, SIGNAL(readyRead()), this, SLOT(gotDataFromSocket()));
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
        this->ds << "QUIT";
        this->socket->flush();
        this->socket->close();
    }

    this->socket->deleteLater();
}

QString SocketHandler::getName()
{
    return this->name;
}

void SocketHandler::gotDataFromSocket()
{
    if (reading) return;
    reading = true;
    for (QString type; this->socket->bytesAvailable() > 0;)
    {
        this->ds >> type;

        if (type == "MSG")
        {
            qDebug("Got Message");
            QString message;
            this->ds >> message;
            this->gotTextMessage(message.prepend("[%1] ").arg(this->name));
        }
        else if (type == "UPD")
        {
            qDebug("Got Update");
            QByteArray buffer;
            this->ds >> buffer;
            this->gotImageUpdate(this->name, buffer);
        }
        else if (type == "ID")
        {
            qDebug("Got ID");
            this->ds >> this->name;
            this->gotTextMessage(QString("[**SERVER**] %1 joined.").arg(this->name));
        }
        else
        {
            qDebug(type.prepend("BIG FUCKING ERROR: ").toAscii().data());
        }
    }
    reading = false;
}

// Some other client sent a text message, please forward to this client
void SocketHandler::sendTextMessage(QString msg)
{   
    this->m.lock();
    this->ds << QString("TXT") << msg;
    this->m.unlock();
}

// Some other client updated image, please forward to this client
void SocketHandler::sendUpdate(QString user, QByteArray buffer)
{
    this->m.lock();
    this->ds << QString("IMG") << user << buffer;
    this->m.unlock();
}
