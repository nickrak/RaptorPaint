#include <QtGui/QApplication>
#include "mainwindow.h"
#include "connectionwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow m;
    m.show();

    return a.exec();
}
