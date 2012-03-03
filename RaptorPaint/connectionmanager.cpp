#include "connectionmanager.h"
#include "connectionwindow.h"

#include <QDebug>

ConnectionManager::ConnectionManager() :
    QThread()
{
}

// Send text message to server
void ConnectionManager::sendTextMessage(QString message)
{
    this->txtQueue.lock();
    this->outboundMessages.enqueue(message);
    this->txtQueue.unlock();
}

// Disconnect from the server
void ConnectionManager::disconnect()
{
    if(this->socket.isOpen())
    {
        QDataStream ds(&this->socket);
        ds << "QUIT";
    }
}

// Open the connection window
void ConnectionManager::openConnectionWindow()
{
    ConnectionWindow* cw = new ConnectionWindow();
    this->connect(cw, SIGNAL(connectionReady(QString,QString)), this, SLOT(connectionWindowResponce(QString,QString)));
    cw->show();
}

// Connect to the server
void ConnectionManager::connectionWindowResponce(QString username, QString hostname)
{
    this->name = username;
    this->start();
    this->socket.moveToThread(this);
    this->socket.connectToHost(hostname, 24554);
}

// Set mute for some user
void ConnectionManager::setMute(QString name, bool mute = true)
{
    mutes[name] = mute;
}

// Get name
QString ConnectionManager::getName()
{
    return this->name;
}

void ConnectionManager::run()
{
    for (; this->socket.thread() != this; this->msleep(10));
    for (; this->socket.state() != QTcpSocket::ConnectedState; this->msleep(10));

    qDebug("Thread Running");

    QDataStream ds(&this->socket);

    ds << QString("ID") << this->name;

    for(keepAlive = true; keepAlive; this->msleep(1000))
    {
        qDebug("Loop");
        for (QString msg("MSG"); !this->outboundMessages.isEmpty();)
        {
            this->txtQueue.lock();
            qDebug("Sending Message");
            ds << msg << this->outboundMessages.dequeue();
            this->txtQueue.unlock();
        }

        if(this->socket.bytesAvailable() > 0)
        {
            qDebug("Got Data");
            QString messageType;
            ds >> messageType;

            // Read text message from server
            if(messageType == "TXT")
            {
                qDebug("--- Got Text Message");
                QString msg;
                ds >> msg;
                int c = msg.indexOf("]");
                QString name = msg.left(c-1).right(c-2);

                // Ignore the message if user has been muted
                if(!mutes[name])
                {
                    this->gotTextMessage(msg);
                }

            }
            else
            {
                qDebug(messageType.prepend("BIG FUCKING ERROR: ").toAscii().data());
            }
        }
    }
}
