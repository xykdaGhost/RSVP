#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CanNet/CanThread.h"
#include "UartNet/SerialPort.h"

extern int GLOBAL_SPEED;
extern int GLOBAL_YOLO;
extern int GLOBAL_TRASH;
extern int GLOBAL_TRASH_NUMBER;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void on_receive(QByteArray tmpdata);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    //the work mode enum
    enum WORK_MODE {
        WORK,
        SHOW,
        DEBUG,
        SAMPLE
    };

    virtual void timerEvent(QTimerEvent *event) override;

    int _workMode;

private:
    Ui::MainWindow *ui;

    void updateParameter();

    int _timerId;

    bool _analysis;

    CanThread* _canThread;

    SerialPort * _serialPort;
};
#endif // MAINWINDOW_H
