#ifndef UART_H
#define UART_H

#include <QSerialPort>
#include <QThread>
#include <QDebug>
#include <QByteArray>

class Uart : public QObject
{
    Q_OBJECT
public:
    static Uart& getInstance(QObject *parent = NULL) {
        static Uart local_port(parent);
        return local_port;
    }
    ~Uart();
    bool connectStauts;
    void init_port();

public slots:
    bool getStauts();
    void handle_data();

private:
    explicit Uart(QObject *parent = NULL);
    QThread *my_thread;
    QSerialPort *port;
};


#endif // UART_H
