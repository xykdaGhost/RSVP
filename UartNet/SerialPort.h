#ifndef SERIALPORT_H
#define SERIALPORT_H
#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QThread>

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
    void ack_heart();

signals:
    void receive_data(QByteArray tmp);

private:
    explicit SerialPort(QObject *parent = NULL);
    QThread *my_thread;
    QSerialPort *port;
};

#endif // SERIALPORT_H
