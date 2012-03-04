#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include <QListWidgetItem>

#include "sockethandler.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow();
    ~MainWindow();

private slots:
    void newConnection();
    void textMessage(QString message);
    
private:
    Ui::MainWindow *ui;

    QTcpServer server;
    QMap<QString, QTcpSocket*> clients;

    void userJoined(QString user);
    void userLeft(QString user);

    QMap<QString, QListWidgetItem*> listItems;
};

#endif // MAINWINDOW_H
