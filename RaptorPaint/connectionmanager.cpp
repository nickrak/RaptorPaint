#include "connectionmanager.h"
#include "connectionwindow.h"

ConnectionManager::ConnectionManager() :
    QThread()
{
    this->socket.moveToThread(this);
}

void ConnectionManager::sendTextMessage(QString message)
{
    if(this->socket.isOpen())
    {
        QDataStream ds(&this->socket);
        ds  << "MSG" << message;
    }
}

void ConnectionManager::disconnect()
{
    if(this->socket.isOpen())
    {
        QDataStream ds(&this->socket);
        ds << "QUIT";
    }
}

void ConnectionManager::openConnectionWindow()
{
    ConnectionWindow* cw = new ConnectionWindow();
    this->connect(cw, SIGNAL(connectionReady(QString,QString)), this, SLOT(connectionWindowResponce(QString,QString)));
    cw->show();
}

void ConnectionManager::connectionWindowResponce(QString username, QString hostname)
{
    this->socket.connectToHost(hostname, 24554);
}

void ConnectionManager::setMute(QString name, bool mute = true)
{
    mutes[name] = mute;
}

QString ConnectionManager::getName()
{
    return this->name;
}

void ConnectionManager::run()
{
    QDataStream ds(&this->socket);
    for(keepAlive = true; keepAlive;)
    {
        if(this->socket.bytesAvailable() > 0)
        {
            QString messageType;
            ds >> messageType;

            if(messageType == "TXT")
            {
                QString msg;
                ds >> msg;
                int c = msg.indexOf("]");
                QString name = msg.left(c-1).right(c-2);

                if(!mutes[name])
                {
                    this->gotTextMessage(msg);
                }

            }
        }
    }
}
