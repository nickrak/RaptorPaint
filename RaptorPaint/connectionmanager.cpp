#include "connectionmanager.h"
#include "connectionwindow.h"

#include <QDebug>
#include <QBuffer>
#include "glwindow.h"

ConnectionManager::ConnectionManager() :
    QObject(), my_Image(QImage(WIDTH, HEIGHT, QImage::Format_ARGB32_Premultiplied))
{
    this->sendImage = false;
#ifdef LOCAL_TEST
    this->connectionWindowResponce("localuser", "");
#endif
}

// Send text message to server
void ConnectionManager::sendTextMessage(QString message)
{
    if (message == "!")
    {
        this->sendImage = true;
        return;
    }

    this->txtQueue.lock();
    QBuffer b;
    b.open(QIODevice::WriteOnly);
    {
        QDataStream ds(&b);
        ds << QString("MSG") << message;
    }
    this->bt->sendBuffer(b);
    this->txtQueue.unlock();
}

// Disconnect from the server
void ConnectionManager::disconnect()
{
    if(this->socket.isOpen())
    {
        delete this->bt;
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
    this->socket.connectToHost(hostname, 24554);
    this->bt = new BufferedTube(&this->socket);
    this->connect(this->bt, SIGNAL(gotBuffer(QByteArray)), this, SLOT(gotDataFromBufferedTube(QByteArray)));

    QBuffer b;
    b.open(QIODevice::WriteOnly);
    {
        QDataStream ds(&b);
        ds << QString("ID") << this->name;
    }
    this->bt->sendBuffer(b);

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

void ConnectionManager::gotDataFromBufferedTube(QByteArray buffer)
{
    QDataStream ds(buffer);

    QString messageType;
    ds >> messageType;

    qDebug(messageType.toAscii().data());

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
        QString name;
        QImage buffer;
        ds >> name >> buffer;

        this->userJoined(name);

        if (!this->mutes[name])
        {
            if (this->name != name)
            {
                if (this->layers.contains(name) && this->layers[name] != 0)
                {
                    QImage* oldImage = this->layers[name];
                    this->layers[name] = new QImage(buffer);
                    delete oldImage;
                }
                else
                {
                    this->layers[name] = new QImage(buffer);
                }
            }
        }
        else
        {
            if (this->layers.contains(name))
            {
                QImage* oldImage = this->layers[name];
                if (this->layers.remove(name) > 0)
                {
                    delete oldImage;
                }
            }
        }
        this->repaintNow();
    }
    else
    {
        qDebug(messageType.prepend("BIG FUCKING ERROR: ").toAscii().data());
    }
}

void ConnectionManager::sendImageUpdate()
{
    if (this->socket.isOpen())
    {
        this->sendImage = false;
        QBuffer b;
        b.open(QIODevice::WriteOnly);
        {
            QDataStream ds(&b);
            ds << QString("UPD") << this->my_Image;
        }
        this->bt->sendBuffer(b);
        qDebug("OK");
    }
}
