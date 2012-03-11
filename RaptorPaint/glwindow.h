#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QGLWidget>
#include <QWidget>
#include <QGLFunctions>

class GLWindow : public QGLWidget
{
    Q_OBJECT
public:
    GLWindow(QWidget *parent);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void setImageStack(QMap<QString, QImage>* stack);

private:
    QMap<QString, QImage>* stack;
};

#endif // GLWINDOW_H
