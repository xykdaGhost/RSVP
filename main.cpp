#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QThread::sleep(2);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
