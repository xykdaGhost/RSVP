#include "SerialPort.h"

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
    data.resize(4);
    data[0] = 0x61;
    data[1] = 0xed;
    data[2] = 0x00;
    data[3] = 0x4e;
    port->write(data, 4);
    qDebug() << "write_id is:" << QThread::currentThreadId();
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



uchar calc(uchar * data) {

}
