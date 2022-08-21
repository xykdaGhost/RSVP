#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UartNet/Uart.h"
#include "JsonWork/ParamManage.h"
#include "Delegates/ValueDelegate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Uart * myUart;
    void updateParameter();
};

#endif // MAINWINDOW_H
