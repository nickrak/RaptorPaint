#include "connectionmanager.h"
#include "connectionwindow.h"

#include <QDebug>
#include "glwindow.h"

#define LOCAL_TEST

ConnectionManager::ConnectionManager() :
    QThread()
{
#ifdef LOCAL_TEST
    this->connectionWindowResponce("localuser", "");
#endif
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
#ifdef LOCAL_TEST
    this->name = username;
#else
    this->name = username;
    this->start();
    this->socket.moveToThread(this);
    this->socket.connectToHost(hostname, 24554);
#endif
    this->layers[this->name] = &this->my_Image;
}

// Toggles mute for the specified user and returns mute status
bool ConnectionManager::toggleMute(QString name)
{
    if (name != this->name)
    {
        bool newMute = !this->mutes[name];
        this->mutes[name] = newMute;
        return newMute;
    }
    return false;
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

    QDataStream ds(&this->socket);

    ds << QString("ID") << this->name;

    for(keepAlive = true; keepAlive; this->msleep(100))
    {
        for (QString msg("MSG"); !this->outboundMessages.isEmpty();)
        {
            this->txtQueue.lock();
            ds << msg << this->outboundMessages.dequeue();
            this->txtQueue.unlock();
        }

        if(this->socket.bytesAvailable() > 0)
        {
            QString messageType;
            ds >> messageType;

            // Read text message from server
            if(messageType == "TXT")
            {
                QString msg;
                ds >> msg;
                int c = msg.indexOf("]");
                QString name = msg.left(c).right(c-1);

                if (name != "**SERVER**")
                {
                    this->userJoined(name);
                }

                qDebug(QString(name).prepend("TXT from ").toAscii().data());
                if (name == "**SERVER**")
                {
                    QStringList parts = msg.split(" ", QString::SkipEmptyParts);

                    if (parts.last() == "joined.")
                    {
                        QString name = parts[parts.count() - 2];
                        this->userJoined(name);
                    }
                    else if (parts.last() == "left.")
                    {
                        QString name = parts[parts.count() - 2];
                        this->userLeft(name);
                    }

                }

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
