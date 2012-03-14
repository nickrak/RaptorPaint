#include "glwindow.h"
#include <iostream>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QGenericMatrix>

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

void GLWindow::initializeGL()
{
    glClearColor(0.3, 0.3, 0.3, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);
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
    glClear(GL_COLOR_BUFFER_BIT | );
    glDisable(GL_CULL_FACE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double w = (double)this->width();
    double h = (double)this->height();
    double z = (double)this->zoomFactor / 100.0;

    glOrtho(0, w, h, 0, 100, -100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(this->xOffset, this->yOffset, 0);
    glScaled(z, z, 0);

    glColor3f(1, 1, 1);
    glBegin(GL_POLYGON);
    glVertex2f(0, 0);
    glVertex2f(WIDTH, 0);
    glVertex2f(WIDTH, HEIGHT);
    glVertex2f(0, HEIGHT);
    glEnd();

    if (this->stack != 0)
    {
        ImageStack* images = this->stack;
        static unsigned int textureCountLastIteration = 0;
        static unsigned int* texture;

        if (textureCountLastIteration > 0)
        {
            glDeleteTextures(textureCountLastIteration, texture);
            free(texture);
        }

        texture = (unsigned int *)malloc(sizeof(unsigned int) * images->count());

        glGenTextures(images->count(), texture);


        textureCountLastIteration = images->count();
        for (int i = 0; i < textureCountLastIteration; i++)
        {
            QImage img = *(images->values().at(i));
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, convertToGLFormat(img).bits());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }

        for (int i = 0; i < textureCountLastIteration; i++)
        {
            glColor4f(1, 1, 1, 0.5);
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glBegin(GL_POLYGON);
            glVertex2f(0, 0);
            glTexCoord2i(0, 0);
            glVertex2f(WIDTH, 0);
            glTexCoord2i(WIDTH, 0);
            glVertex2f(WIDTH, HEIGHT);
            glTexCoord2i(WIDTH, HEIGHT);
            glVertex3f(0, HEIGHT, 50);
            glTexCoord2i(0, HEIGHT);
            glEnd();
        }

    }
}
