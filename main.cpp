#include "mainwindow.h"

#include <QApplication>
#include <QPushButton>
#include <iostream>

int main(int argc, char *argv[])try
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}catch(std::exception &e){
    std::cout<<e.what();
}
