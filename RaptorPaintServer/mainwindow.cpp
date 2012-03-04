#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

// Constructor
MainWindow::MainWindow() :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->server.listen(QHostAddress::Any, 24554);

    this->connect(&this->server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

// Server got new connection from a client
void MainWindow::newConnection()
{
    QTcpSocket* sock = this->server.nextPendingConnection();

    for (; sock->state() != QTcpSocket::ConnectedState; )
    {
        qDebug(QString::number((int)sock->state()).toAscii().data());
    }

    qDebug("Got Connection!");
    SocketHandler* sh =  new SocketHandler(sock);
    this->connect(sh, SIGNAL(gotTextMessage(QString)),this,SLOT(textMessage(QString)));
}

void MainWindow::textMessage(QString message)
{
    ui->textEdit->append(message);


    int c = message.indexOf("]");
    QString name = message.left(c).right(c-1);

    if (name != "**SERVER**")
    {
        this->userJoined(name);
    }

    qDebug(QString(name).prepend("TXT from ").toAscii().data());
    if (name == "**SERVER**")
    {
        QStringList parts = message.split(" ", QString::SkipEmptyParts);

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
}

void MainWindow::userJoined(QString name)
{
    if (this->listItems.contains(name)) return;
    QListWidgetItem* item = new QListWidgetItem(name, ui->userList);
    this->listItems[name] = item;
}

void MainWindow::userLeft(QString name)
{
    if (this->listItems.contains(name))
    {
        QListWidgetItem* item = this->listItems[name];
        this->listItems.remove(name);
        delete item;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
