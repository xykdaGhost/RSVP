#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "UartNet/Uart.h"
#include "JsonWork/ParamManage.h"
#include "Delegates/ValueDelegate.h"
#include "TableModel/ResultModel.h"
#include <QDir>
#include <opencv4/opencv2/opencv_modules.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "Camera/FileCamera.h"
#include "Camera/GenCamera.h"

#include <QThread>

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
    ParamManage param;
    bool _analysis;
};

#endif // MAINWINDOW_H
