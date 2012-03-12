#include "glwindow.h"
#include <iostream>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QGenericMatrix>

#define WIDTH 800
#define HEIGHT 480

GLWindow::GLWindow(QWidget* parent) :
    QGLWidget(parent), stack(0)
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

void GLWindow::setImageStack(QMap<QString, QImage>* stack = 0)
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
    offset(0, 2) = this->xOffset;
    offset(1, 2) = this->yOffset;

    scale.setToIdentity();
    scale(0, 0) = this->zoomFactor;
    scale(1, 1) = this->zoomFactor;

    rotate.setToIdentity();


    QGenericMatrix<1, 3, double> coords;
    coords(0, 0) = rx;
    coords(1, 0) = ry;
    coords(2, 0) = 1.0;

    coords = offset * scale * rotate * coords;

    this->drawHere(coords(0, 0), coords(1, 0));
}

void GLWindow::initializeGL()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void GLWindow::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double w = (double)this->width();
    double h = (double)this->height();
    double z = (double)this->zoomFactor / 100.0;

    glOrtho(0, w, h, 0, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(this->xOffset, this->yOffset, 0);
    glScaled(z, z, 1);

    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex3i(0, 0, 100);
    glVertex3i(WIDTH, 0, 100);
    glVertex3i(WIDTH, HEIGHT, 100);
    glVertex3i(0, HEIGHT, 100);
    glEnd();

    if (this->stack != 0)
    {
        QMap<QString, QImage>* images = this->stack;
        foreach (QString key, images->keys())
        {
            QImage gldata = QGLWidget::convertToGLFormat((*images)[key]);
            glDrawPixels(gldata.width(), gldata.height(), GL_RGBA, GL_UNSIGNED_BYTE, gldata.bits());
        }
    }
}
