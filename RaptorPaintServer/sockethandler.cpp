#include "sockethandler.h"
#include <QDebug>
#include <QBuffer>

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
    readMutex.lock();
    for (QString type; !this->ds.atEnd();)
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
            int size;
            for (this->ds >> size; this->socket->bytesAvailable() < size; );
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
            this->status(ds);
            qDebug(type.prepend("BIG FUCKING ERROR: ").append("^").append(type.toAscii().toHex()).append("$")
                   .append(QString::number(this->socket->bytesAvailable())).toAscii().data());
            exit(1);

        }
    }
    readMutex.unlock();
}

// Some other client sent a text message, please forward to this client
void SocketHandler::sendTextMessage(QString msg)
{   
    this->writeMutex.lock();
    this->ds << QString("TXT") << msg;
    this->writeMutex.unlock();
}

// Some other client updated image, please forward to this client
void SocketHandler::sendUpdate(QString user, QByteArray buffer)
{
    if (user != this->name)
    {
        this->writeMutex.lock();
        this->ds << QString("IMG") << user << buffer.size() << buffer;
        this->writeMutex.unlock();
    }
}
