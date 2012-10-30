// Raptor Paint
// Copyright (C) 2012 Nick Rakoczy, Jessica Randall
//
// This program is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <http://www.gnu.org/licenses/>.

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
