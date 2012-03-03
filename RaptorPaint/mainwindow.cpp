#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager)
{
    ui->setupUi(this);
    this->connect(this->cm,SIGNAL(gotTextMessage(QString)),this,SLOT(gotTextMessage(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
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
