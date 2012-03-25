#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionmanager.h"
#include "glwindow.h"

#include <QMap>
#include <QPainter>
#include <QListWidgetItem>

namespace Ui {
class MainWindow;
}

typedef enum
{
    BRUSH = 0,
    PENCIL = 1,
    ERASER = 2,
    TYPE = 3
} RaptorPaintTool;

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
    void userMuteToggle(QListWidgetItem* item);

    void drawHere(double x, double y);

    void saveToFile();
    
private:
    void changeCanvas(double x, double y, QPainter *painter);

    Ui::MainWindow *ui;
    ConnectionManager *cm;

    QIcon muted;
    QIcon unmuted;

    RaptorPaintTool selectedTool;
    int selectedColor;
    int toolSize;

    bool wasDragging;

    QMap<QString, QListWidgetItem*> listItems;
};

#endif // MAINWINDOW_H
