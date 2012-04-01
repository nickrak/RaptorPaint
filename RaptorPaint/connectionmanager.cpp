#include "connectionmanager.h"
#include "connectionwindow.h"

#include <QDebug>
#include "glwindow.h"

ConnectionManager::ConnectionManager() :
    QThread(), my_Image(QImage(WIDTH, HEIGHT, QImage::Format_ARGB32_Premultiplied))
{
#ifdef LOCAL_TEST
    this->connectionWindowResponce("localuser", "");
#endif
}

QByteArray ConnectionManager::compressImage()
{
    QByteArray buffer;
    QDataStream ds(&buffer, QIODevice::ReadWrite);
    ds << *this->myImage();
    return qCompress(buffer);
}

void ConnectionManager::decompressImage(QString user, QByteArray data)
{
    QDataStream ds(qUncompress(data));
    QImage img;
    ds >> img;

    QImage* oldImage = this->layers[user];
    QImage* newImage = new QImage(img);

    this->layers[user] = newImage;

    if (oldImage)
    {
        delete oldImage;
    }
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

ImageStack* ConnectionManager::getLayerPtr()
{
    return &this->layers;
}

QImage* ConnectionManager::myImage()
{
    return &this->my_Image;
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

        QString upd("UPD");
        ds << upd << this->compressImage();

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
                if(!this->mutes[name])
                {
                    this->gotTextMessage(msg);
                }
            }
            else if (messageType == "IMG")
            {
                QByteArray buffer;
                QString name;
                ds >> name >> buffer;

                if (!this->mutes[name])
                {
                    this->decompressImage(name, buffer);
                }
                else
                {
                    this->layers.remove(name);
                }
            }
            else
            {
                qDebug(messageType.prepend("BIG FUCKING ERROR: ").toAscii().data());
            }
        }
    }
}
