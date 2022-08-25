#include "Uart.h"

Uart::Uart(QObject *parent) : QObject(parent) {
    my_thread = new QThread();
    port = new QSerialPort();
    init_port();
    my_thread->start();
    connectStauts = false;
}

Uart::~Uart() {

    port->close();
    port->deleteLater();
    my_thread->quit();
    my_thread->wait();
    my_thread->deleteLater();
}

void Uart::init_port() {
    port->setPortName("/dev/ttyTHS0");
    port->setBaudRate(QSerialPort::Baud115200);
    port->setDataBits(QSerialPort::Data8);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    port->setParity(QSerialPort::OddParity);

    if (port->open(QIODevice::ReadWrite)) {
        qDebug() << "Port have been opened";
        connectStauts = true;
    } else {
        qDebug() << "open it failed";
    }
    connect(port, SIGNAL(readyRead()), this, SLOT(handle_data()), Qt::QueuedConnection);
}

bool Uart::getStauts() {
    return connectStauts;
}

void SerialPort::writeArray(int len, const uchar * array) {
    qDebug() << "write_id is:" << QThread::currentThreadId();
    port->write((const char *)array, len);
}

void Uart::handle_data() {
    QByteArray data = port->readAll();
    qDebug() << QStringLiteral("data received(收到的数据):") << data;
    qDebug() << "handing thread is:" << QThread::currentThreadId();

}

int Uart::msgAnalysis(QByteArray data) {
    int commandKeyWord = (data[2] << 8) + data[3];

    switch (commandKeyWord)
    {
    case COMMAND_SEARCH:
        writeArray(6, ACK_SEARCH);
        break;
    case COMMAND_GETSHOOT:
        GenCamera::getInstance().acquireImage(ResultModel result);
        break;
    case COMMAND_CONTROL:
        qDebug() << "收到停止/启动";
    default:
        break;
    }
}
