#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QList>

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
    
private:
    Ui::MainWindow *ui;

    QTcpServer server;
    QMap<QString, QTcpSocket*> clients;
};

#endif // MAINWINDOW_H
