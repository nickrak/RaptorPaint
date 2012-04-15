#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "connectionmanager.h"
#include "glwindow.h"

#include <QMap>
#include <QPainter>
#include <QListWidgetItem>
#include <QColorDialog>

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
    void initialize();

private slots:
    void gotTextMessage(QString msg);
    void mnuConnect();
    void txtInputReturnPressed();

    void userLeft(QString name);
    void userJoined(QString name);
    void userMuteToggle(QListWidgetItem* item);

    void drawHere(double x, double y);

    void saveToFile();

    void mouseRelease();

    void clearCanvas();

    void setToBrush();
    void setToPencil();
    void setToEraser();
    void setToType();

    void openColorPicker();
    void colorSelected(QColor color);

    void increaseBrush();
    void decreaseBrush();
    void resetBrush();

    void blendingModeChanged(int index);
    
private:
    bool changeCanvas(double x, double y, QPainter *painter);
    inline void resetToolbox();
    void setPaintCursor();
    void setPaintCursorSize(double size = 1.0);

    Ui::MainWindow *ui;
    ConnectionManager *cm;

    QIcon muted;
    QIcon unmuted;

    QCursor currentSizeCursor;

    RaptorPaintTool selectedTool;
    QColor selectedColor;
    int toolSize;
    QPainter::CompositionMode blendMode;

    bool wasDragging;

    QMap<QString, QListWidgetItem*> listItems;

    QColorDialog qcd;
};

#endif // MAINWINDOW_H
