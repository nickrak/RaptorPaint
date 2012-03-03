#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connectionwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager)
{
    ui->setupUi(this);
    this->connect(this->cm,SIGNAL(gotTextMessage(QString)),this,SLOT(gotTextMessage(QString)));
    this->connect(ui->actionConnect_Host, SIGNAL(triggered()), this, SLOT(mnuConnect()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mnuConnect()
{
    this->cm->openConnectionWindow();
}

// Got message from server
void MainWindow::gotTextMessage(QString msg)
{
    if(msg.startsWith(QString("[%1]").arg(this->cm->getName())))
    {
        msg = msg.prepend("<p class=\"me\">").append("</p>");
    }
    ui->chatLog->append(msg);
}
