#include "sockethandler.h"

// Newly received connection
SocketHandler::SocketHandler(QTcpSocket* sock) :
    QObject(), ds(sock)
{
    this->socket = sock;

    foreach (SocketHandler* handler, socketHandlers)
    {
        this->connect(this, SIGNAL(gotImageUpdate(QString,QByteArray)), handler, SLOT(sendUpdate(QString,QByteArray)));
        this->connect(this, SIGNAL(gotTextMessage(QString)), this, SLOT(sendTextMessage(QString)));
    }

    this->connect(this->socket, SIGNAL(readyRead()), this, SLOT(gotDataFromSocket()));

    socketHandlers << this;
}

// Connection Lost
SocketHandler::~SocketHandler()
{
    socketHandlers.removeAll(this);

    if (this->socket->isOpen())
    {
        this->ds << "QUIT";
        this->socket->flush();
        this->socket->close();
    }

    this->socket->deleteLater();
}

void SocketHandler::gotDataFromSocket()
{
    for (QString type; this->socket->bytesAvailable() > 0;)
    {
        this->ds >> type;

        if (type == "MSG")
        {
            QString message;
            this->ds >> message;
            this->gotTextMessage(message.prepend("[%1] ").arg(this->name));
        }
        else if (type == "UPD")
        {
            QByteArray buffer;
            this->ds >> buffer;
            this->gotImageUpdate(this->name, buffer);
        }
        else if (type == "ID")
        {
            this->ds >> this->name;
            this->gotTextMessage(QString("[**SERVER**] %1 joined.").arg(this->name));
        }
    }
}

// Some other client sent a text message, please forward to this client
void SocketHandler::sendTextMessage(QString msg)
{   
    this->m.lock();
    this->ds << "TXT" << msg;
    this->m.unlock();
}

// Some other client updated image, please forward to this client
void SocketHandler::sendUpdate(QString user, QByteArray buffer)
{
    this->m.lock();
    this->ds << "IMG" << user << buffer;
    this->m.unlock();
}
