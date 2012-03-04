#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "connectionwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    cm(new ConnectionManager),
    muted(QIcon(":/userIcons/muted.png")),
    unmuted(QIcon(":/userIcons/unmuted.png"))
{
    ui->setupUi(this);
    this->connect(this->cm, SIGNAL(gotTextMessage(QString)), this, SLOT(gotTextMessage(QString)));
    this->connect(ui->actionConnect_Host, SIGNAL(triggered()), this, SLOT(mnuConnect()));
    this->connect(ui->input, SIGNAL(returnPressed()), this, SLOT(txtInputReturnPressed()));

    this->connect(this->cm, SIGNAL(userJoined(QString)), this, SLOT(userJoined(QString)));
    this->connect(this->cm, SIGNAL(userLeft(QString)), this, SLOT(userLeft(QString)));

    this->connect(ui->userList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(userMuteToggle(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::txtInputReturnPressed()
{
    this->cm->sendTextMessage(ui->input->text());
    ui->input->setText("");
}

void MainWindow::userMuteToggle(QListWidgetItem *item)
{
    QString user = item->text();
    bool mute = this->cm->toggleMute(user);
    item->setIcon(mute ? this->muted : this->unmuted);
}

void MainWindow::mnuConnect()
{
    this->cm->openConnectionWindow();
}

void MainWindow::userJoined(QString name)
{
    qDebug("Join");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name)) return;
    QListWidgetItem* item = new QListWidgetItem(this->unmuted, name, ui->userList);
    this->listItems[name] = item;
}

void MainWindow::userLeft(QString name)
{
    qDebug("Leave");
    qDebug(name.toAscii().data());
    if (this->listItems.contains(name))
    {
        QListWidgetItem* item = this->listItems[name];
        this->listItems.remove(name);
        delete item;
    }
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
