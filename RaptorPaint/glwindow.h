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

    void mouseMoveEvent(QMouseEvent* e);

public slots:
    void adjustZoom(double amount);
    void adjustOffset(double x, double y);

private:
    QMap<QString, QImage>* stack;

    double xOffset;
    double yOffset;
    double zoomFactor;
};

#endif // GLWINDOW_H
