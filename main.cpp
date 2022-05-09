#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <iostream>
#include <QtTest/QTest>

int main(int argc, char *argv[])try
{
    QThread::sleep(1);
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    return a.exec();
}catch(std::exception &e){
    std::cout<<e.what();
}
