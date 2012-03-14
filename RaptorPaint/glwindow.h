#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QWidget>
#include <QGenericMatrix>
#include <iostream>


#define WIDTH 1024
#define HEIGHT 1024

typedef QMap<QString, QImage*> ImageStack;

class GLWindow : public QWidget
{
    Q_OBJECT
public:
    GLWindow(QWidget *parent);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void setImageStack(ImageStack* stack);

    void mouseMoveEvent(QMouseEvent* e);

public slots:
    void zoomIn();
    void zoomOut();
    void adjustOffset(double x, double y);

signals:
    void drawHere(double x, double y);

private:
    ImageStack* stack;

    QPoint lastLocation;

    double xOffset;
    double yOffset;
    double zoomFactor;

    template<int N, int M, class T>
    static void printMatrix(QGenericMatrix<N,M,T> m)
    {
        std::cout << "[\n";
        for (int row = 0; row < M; row++)
        {
            for (int col = 0; col < N; col++)
            {
                if (col > 0) std::cout << ", ";
                std::cout << m(row, col);
            }
            std::cout << "\n";
        }
        std::cout << "]" << std::endl;
    }
};

#endif // GLWINDOW_H
