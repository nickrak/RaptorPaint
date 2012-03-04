#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionmanager.h"

#include <QMap>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void gotTextMessage(QString msg);
    void mnuConnect();
    void txtInputReturnPressed();

    void userLeft(QString name);
    void userJoined(QString name);
    
private:
    Ui::MainWindow *ui;
    ConnectionManager *cm;

    QMap<QString, QListWidgetItem*> listItems;
};

#endif // MAINWINDOW_H
