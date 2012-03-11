#include "glwindow.h"
#include <iostream>

GLWindow::GLWindow(QWidget* parent) :
    QGLWidget(parent), stack(0)
{

}

void GLWindow::setImageStack(QMap<QString, QImage>* stack = 0)
{
    this->stack = stack;
}

void GLWindow::initializeGL()
{
    glClearColor(1, 1, 1, 0);
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double w = (double)this->width();
    double h = (double)this->height();

    std::cout << w << std::endl;
    std::cout << h << std::endl;

    glOrtho(0, w, h, 0, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
