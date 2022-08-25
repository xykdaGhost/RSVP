
#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QThread>
#include "./TreeModel/ParameterModel.h"
#include "./JsonWork/ParamManage.h"
#include "./TableModel/ResultModel.h"

class SerialPort : public QObject
{
    Q_OBJECT
public:
    static SerialPort& getInstance(QObject *parent = NULL) {
        static SerialPort local_port(parent);
        return local_port;
    }
    ~SerialPort();

    void init_port();

public slots:
    void handle_data();
    void write_data();
    void ask_date();
    void ask_shoot();
    void ack_shoot();
    void ack_date();
    void ack_heart();
    void ack_mode();
    void ack_param();
    void ack_speed();
    void ack_level();
    void ack_status();
    void ack_save();
    void ask_interval(short saveInterval, short shootInterval);
    void ask_yoloANDsave(char stauts);
    void ask_reset();
    void ask_mode(char mode);
    void ask_runANDstop(char m);
    void ack_search();

signals:
    void receive_data(QByteArray tmp);

private:
    explicit SerialPort(QObject *parent = NULL);
    QThread *my_thread;
    QSerialPort *port;
};

#endif // SERIALPORT_H
