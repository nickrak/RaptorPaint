#include "sockethandler.h"

// Newly received connection
SocketHandler::SocketHandler(QTcpSocket* sock) :
    QThread(), ds(sock)
{
    this->socket = sock;

    this->start();

    this->socket->moveToThread(this);

    foreach (SocketHandler* handler, socketHandlers)
    {
        this->connect(this, SIGNAL(gotImageUpdate(QString,QByteArray)), handler, SLOT(sendUpdate(QString,QByteArray)));
        this->connect(this, SIGNAL(gotTextMessage(QString)), this, SLOT(sendTextMessage(QString)));
    }

    socketHandlers << this;
}

// Connection Lost
SocketHandler::~SocketHandler()
{
    socketHandlers.removeAll(this);

    this->keepAlive = false;
    this->wait();

    if (this->socket->isOpen())
    {
        this->ds << "QUIT";
        this->socket->flush();
        this->socket->close();
    }

    this->socket->deleteLater();
}

void SocketHandler::run()
{
    // Wait for socket to be transfered to this thread
    for (; this->socket->thread() != this; this->usleep(10));

    for (this->keepAlive = true; this->keepAlive; this->usleep(100))
    {

        // Send all pending txt messages to client
        for (const QString txt("TXT"); !this->pendingMessages.isEmpty(); )
        {
            this->ds << txt << this->pendingMessages.dequeue();
        }

        // Send all pending image updates to client
        foreach (QString user, this->pendingUpdates.keys())
        {
            for (const QString img("IMG"); !this->pendingUpdates[user].isEmpty(); )
            {
                this->ds << img << this->pendingUpdates[user].dequeue();
            }
        }

        // Read all incoming messages from clients
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
}

// Some other client sent a text message, please forward to this client
void SocketHandler::sendTextMessage(QString msg)
{
    this->pendingMessages << msg;
}

// Some other client updated image, please forward to this client
void SocketHandler::sendUpdate(QString user, QByteArray buffer)
{
    if (!this->pendingUpdates.contains(user)) this->pendingUpdates[user] = QQueue<QByteArray>();
    this->pendingUpdates[user].enqueue(buffer);
}
