#ifndef UART_H
#define UART_H

#include <QSerialPort>
#include <QThread>
#include <QDebug>
#include <QByteArray>
#include "../TableModel/ResultModel.h"

#define COMMAND_SEARCH          0x6301
#define COMMAND_HEARTPACKAGE    0x6320
#define COMMAND_CONTROL         0x0010
#define COMMAND_STATUS          0x0011
#define COMMAND_SYNC            0x0012
#define COMMAND_SETWORKMODE     0x0020
#define COMMAND_GETWORKMODE     0x0023
#define COMMAND_SYNCWORKMODE    0x0026
#define COMMAND_SETTIME         0x0040
#define COMMAND_GETTIME         0x0041
#define COMMAND_SETPARAM        0x0050
#define COMMAND_GETPARAM        0x0053
#define COMMAND_SYNCPARAM       0x0058
#define COMMAND_SENDRESULT      0x0062
#define COMMAND_SETSAVEMODE     0x0070
#define COMMAND_GETSAVEMODE     0x0072
#define COMMAND_SYNCSAVEMODE    0x0075
#define COMMAND_GETCAMERASTATUS 0x0702
#define COMMAND_GETSPEED        0x0080
#define COMMAND_GETSHOOT        0x0090 
#define COMMAND_GETSYSTEMINFO   0x00B1
#define COMMAND_RECOVERY        0x00B5



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
    void writeArray(int len, const char * array);
    void init_port();

public slots:
    bool getStauts();
    void handle_data();

private:
    explicit Uart(QObject *parent = NULL);
    QThread *my_thread;
    QSerialPort *port;

    const uchar ACK_SEARCH[] = {0xea, 0x02, 0xE3, 0x01, 0xE4, 0xEB};
};


#endif // UART_H
