#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow() :
    QMainWindow(),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->server.listen(QHostAddress::Any, 24554);

    this->connect(&this->server, SIGNAL(newConnection()), this, SLOT(newConnection()));
}

void MainWindow::newConnection()
{
    QTcpSocket* sock = this->server.nextPendingConnection();
    new SocketHandler(sock);
}

MainWindow::~MainWindow()
{
    delete ui;
}
