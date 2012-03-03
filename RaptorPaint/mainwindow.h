#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionmanager.h"

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
    
private:
    Ui::MainWindow *ui;
    ConnectionManager *cm;
};

#endif // MAINWINDOW_H
