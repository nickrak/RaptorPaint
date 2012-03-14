#include "glwindow.h"
#include <iostream>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QGenericMatrix>

GLWindow::GLWindow(QWidget* parent = 0) :
    QWidget(parent), stack(0)
{
    this->xOffset = 00.0;
    this->yOffset = 00.0;
    this->zoomFactor = 100.0;
    this->setMouseTracking(true);
}

void GLWindow::zoomIn()
{
    this->zoomFactor *= 1.25;
    this->repaint();
}

void GLWindow::zoomOut()
{
    this->zoomFactor *= 0.75;

    if (this->zoomFactor <= 1)
    {
        this->zoomFactor = 1;
    }

    this->repaint();
}

void GLWindow::setImageStack(ImageStack* stack = 0)
{
    this->stack = stack;
}

void GLWindow::adjustOffset(double x, double y)
{
    this->xOffset += x;
    this->yOffset += y;
    this->repaint();
}

void GLWindow::mouseMoveEvent(QMouseEvent* e)
{
    // Move Event
    bool moveThisCycle = QApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) && e->buttons().testFlag(Qt::LeftButton);
    double rx = e->pos().x();
    double ry = e->pos().y();

    if (moveThisCycle)
    {
        double dx = rx - this->lastLocation.x();
        double dy = ry - this->lastLocation.y();
        this->adjustOffset(dx, dy);
    }

    this->lastLocation.setX(rx);
    this->lastLocation.setY(ry);

    // Tools
    QGenericMatrix<3, 3, double> offset;
    QGenericMatrix<3, 3, double> scale;
    QGenericMatrix<3, 3, double> rotate;

    offset.setToIdentity();
    offset(0, 2) = -this->xOffset;
    offset(1, 2) = -this->yOffset;

    scale.setToIdentity();
    scale(0, 0) = 100.0 / this->zoomFactor;
    scale(1, 1) = 100.0 / this->zoomFactor;

    rotate.setToIdentity();

    QGenericMatrix<1, 3, double> coords;
    coords(0, 0) = rx;
    coords(1, 0) = ry;
    coords(2, 0) = 1.0;

    QGenericMatrix<1, 3, double> n =  scale * offset * rotate * coords;

    this->drawHere(n(0, 0), n(1, 0));
}
