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
    qDebug("Sockethandler made!");
    this->connect(sh, SIGNAL(gotTextMessage(QString)),this,SLOT(textMessage(QString)));
}

void MainWindow::textMessage(QString message)
{
    ui->textEdit->append(message);
}

MainWindow::~MainWindow()
{
    delete ui;
}
