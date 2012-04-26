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

#ifndef GLWINDOW_H
#define GLWINDOW_H

#include <QWidget>
#include <QGenericMatrix>
#include <iostream>
#include <QImage>


#define WIDTH 1024
#define HEIGHT 1024

typedef QMap<QString, QImage*> ImageStack;

class GLWindow : public QWidget
{
    Q_OBJECT
public:
    GLWindow(QWidget *parent);
    void setImageStack(ImageStack* stack);

    void mouseMoveEvent(QMouseEvent* e);

    void paintEvent(QPaintEvent* e);

    void scroll(int dx, int dy);

public slots:
    void zoomIn();
    void zoomOut();
    void zoomActual();
    void adjustOffset(double x, double y);

signals:
    void drawHere(double x, double y);
    void mouseRelease();

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
