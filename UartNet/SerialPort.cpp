
#include "SerialPort.h"

extern int GLOBAL_TRASH_AMOUNT;
extern int GLOBAL_TRASH_DENSITY;

SerialPort::SerialPort(QObject *parent) : QObject(parent) {

    my_thread = new QThread();
    port = new QSerialPort();
    init_port();
//    this->moveToThread(my_thread);
//    port->moveToThread(my_thread);
    my_thread->start();

}

SerialPort::~SerialPort() {

    port->close();
    port->deleteLater();
    my_thread->quit();
    my_thread->wait();
    my_thread->deleteLater();
}

void SerialPort::init_port() {
    port->setPortName("/dev/ttyTHS0");
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setParity(QSerialPort::OddParity);

    if (port->open(QIODevice::ReadWrite)) {
        qDebug() << "Port have been opened";
    } else {
        qDebug() << "open it failed";
    }
    connect(port, SIGNAL(readyRead()), this, SLOT(handle_data()), Qt::QueuedConnection);

}

void SerialPort::handle_data() {
    QByteArray data = port->readAll();
    qDebug() << QStringLiteral("data received(收到的数据):") << data;
    qDebug() << "handing thread is:" << QThread::currentThreadId();
    emit receive_data(data);
}

void SerialPort::write_data() {
    qDebug() << "write_id is:" << QThread::currentThreadId();
    char data[3] = {0xaa, 0xaa, 0xaa};
    port->write(QByteArray(data), 3);
//    port->write("aa", 2);
}


void SerialPort::ask_date() {
    qDebug() << "write_id is:" << QThread::currentThreadId();
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x00;
    data[3] = 0x41;
    data[4] = 0x41;
    data[5] = 0xeb;
    port->write(data, 6);
}

void SerialPort::ack_date() {
    qDebug() << "write_id is:" << QThread::currentThreadId();
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x80;
    data[3] = 0x40;
    data[4] = 0xc0;
    data[5] = 0xeb;
    port->write(data, 6);
}

void SerialPort::ask_shoot() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x00;
    data[3] = 0x90;
    data[4] = 0x90;
    data[5] = 0xeb;
    port->write(data, 6);
    qDebug() << "ask shoot";
}

void SerialPort::ack_shoot() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x00;
    data[2] = 0x80;
    data[3] = 0x90;
    data[4] = 0x20;
    data[5] = 0xeb;

    port->write(data, 6);

    qDebug() << "ack_shoot success";

}

void SerialPort::ack_heart() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x80;
    data[3] = 0x92;
    data[4] = 0x12;
    data[5] = 0xeb;
    port->write(data, 6);

    qDebug() << "heart checked";
}

void SerialPort::ack_mode() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x80;
    data[3] = 0x20;
    data[4] = 0xa0;
    data[5] = 0xeb;
    port->write(data, 6);

    qDebug() << "ack mode";
}

void SerialPort::ack_param() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x80;
    data[3] = 0x50;
    data[4] = 0xd0;
    data[5] = 0xeb;
    port->write(data, 6);

    qDebug() << "ack param";
}

void SerialPort::ack_speed() {
    QByteArray data;
    data.resize(6);
    data[0] = 0xea;
    data[1] = 0x02;
    data[2] = 0x80;
    data[3] = 0x80;
    data[4] = 0x00;
    data[5] = 0xeb;
    port->write(data, 6);

    qDebug() << "ack speed";
}


void SerialPort::ack_level() {
    QByteArray data;
    data.resize(8);
    data[0] = 0xea;
    data[1] = 0x04;
    data[2] = 0x00;
    data[3] = 0x62;



    data[4] = GLOBAL_TRASH_AMOUNT;
    data[5] = GLOBAL_TRASH_DENSITY;

    data[6] = (data[2] + data[3] + data[4] +data[5])%256;
    data[7] = 0xeb;
    port->write(data, 8);

    qDebug() << "level is " << data;
}

void SerialPort::ack_status() {
    QByteArray data;
    data.resize(8); 

    data[0] = 0xea;
    data[1] = 0x23;   
    data[2] = 0x80;   
    data[3] = 0x91;   
    data[4] = 0x02  
    data[5] = 0x01;   
    data[6] = 0x01;   

    data[7] = 0xea;   
    data[8] = 0xea;   
    data[9] = 0xea;   
    data[10] = 0xea;   
    data[11] = 0xea;   
    data[12] = 0xea;    
    data[13] = ;
    data[14] = ;
    data[15] = ;   
    data[16] = ;     


    data[17] = ;

    data[18] = ;
    data[19] = ;

    data[20] = ;
    data[21] = ;

    data[22] = ;
    data[23] = ;
    
    data[24] = ;


    data[25] = (data[4]+data[5]+data[6]+data[7]+data[8]+data[9]+data[10]+data[11]+data[12]+data[13]+data[14]+data[15]+data[16]+data[17]+data[18]+data[19]+data[20]+data[21]+data[22]+data[23]+data[24])%256;
    data[26] = 0xeb;


}

uchar calc(uchar * data) {

}
