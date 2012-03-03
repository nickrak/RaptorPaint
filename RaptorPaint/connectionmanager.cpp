#include "connectionmanager.h"
#include "connectionwindow.h"

ConnectionManager::ConnectionManager() :
    QThread()
{
}

// Send text message to server
void ConnectionManager::sendTextMessage(QString message)
{
    if(this->socket.isOpen())
    {
        QDataStream ds(&this->socket);
        ds  << "MSG" << message;
    }
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
    this->socket.connectToHost(hostname, 24554);
    this->start();
    this->socket.moveToThread(this);
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
    for (; this->socket.thread() != this; this->usleep(10));

    QDataStream ds(&this->socket);
    for(keepAlive = true; keepAlive;)
    {
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
                QString name = msg.left(c-1).right(c-2);

                // Ignore the message if user has been muted
                if(!mutes[name])
                {
                    this->gotTextMessage(msg);
                }

            }
        }
    }
}
