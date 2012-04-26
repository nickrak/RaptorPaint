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

    ConnectionManager *cm;

private slots:
    void gotTextMessage(QString msg);
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

    void repaintNow();

private:
    bool changeCanvas(double x, double y, QPainter *painter);
    inline void resetToolbox();
    void setPaintCursor();
    void setPaintCursorSize(double size = 1.0);

    Ui::MainWindow *ui;

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
