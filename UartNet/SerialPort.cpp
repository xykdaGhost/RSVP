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
    data.resize(4);
    data[0] = 0x54;
    data[1] = 0xcc;
    data[2] = 0x00;
    data[3] = 0x20;
    port->write(data, 4);
}

void SerialPort::ask_shoot() {
    QByteArray data;
    data.resize(4);
    data[0] = 0x60;
    data[1] = 0xed;
    data[2] = 0x00;
    data[3] = 0x4d;
    port->write(data, 4);
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
