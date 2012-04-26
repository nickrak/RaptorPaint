#include <QtGui/QApplication>
#include "mainwindow.h"
#include "connectionwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow m;
    m.initialize();
    m.cm->openConnectionWindow();
    m.cm->connect(m.cm, SIGNAL(done()), &m, SLOT(show()));

    return a.exec();
}
