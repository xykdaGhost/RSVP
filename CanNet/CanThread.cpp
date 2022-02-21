#include "CanThread.h"
#include <net/if.h>
#include <cstring>
#include <sys/ioctl.h>
#include "linux/can.h"
#include <unistd.h>
#include <QDebug>
#include "CanFrame.h"
#include "../JsonWork/ParamManage.h"

extern int GLOBAL_SPEED;

CanThread::CanThread(QString canDev, QObject* parent) :
    _thread(new QThread(this)) {
    int canfd = -1;
    if((canfd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        qDebug() << "creat can socket fail" << endl;
    }
    _socket = new CanSocket(canfd);
    struct sockaddr_can addr{};
    struct ifreq ifr{};
    strcpy(ifr.ifr_name, canDev.toLatin1().data());
    ioctl(canfd, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    _socket->bindAddress(addr);
    _socket->enableLoop(false);
    _socket->enableRecvOwn(false);

    connect(_thread, &QThread::started, this, &CanThread::recvThread);
    this->moveToThread(_thread);
    _thread->start();
}

void CanThread::recvThread() {
    CanFrame frame;
    while(read(_socket->fd(), frame.getFrame().get(), sizeof(can_frame))) {
        if(!frame.checkError()) {
            qDebug() << "error frame";
            continue;
        }
//        qDebug()<<"can id:"<<frame.getFrame()->can_id<<endl;
//        qDebug()<<"can data:";
//        for(unsigned char i : frame.getFrame()->data)
//            qDebug()<<i<<" ";
//        qDebug()<<endl;
        if((frame.getFrame()->can_id & CAN_EFF_MASK) == 0x18C31726 && frame.getFrame()->can_dlc == 0x08) {
            ParamManage::getInstance().model()->paramStruct().aec.speed = frame.getFrame()->data[7];
            emit speedMessage(frame.getFrame()->data[7]);
            GLOBAL_SPEED = frame.getFrame()->data[7];
        }
    }
}

void CanThread::sendRes(std::vector<std::pair<int, double>> res) {
    double sumDig = 0;
    for(auto i : res) {
        sumDig += i.second;
    }
    sumDig *= 1000;

    CanFrame frame;
    char data[8];
    data[0] = 1;
    data[1] = res[4].first & 0xff;
    data[2] = (res[0].first + res[7].first) & 0xff;
    data[3] = res[6].first & 0xff;
    data[4] = (res[1].first + res[2].first + res[5].first + res[8].first) & 0xff;
    data[5] = res[3].first & 0xff;
    data[6] = (sumDig > 255)? 255:sumDig;
    data[7] = 0;

    frame.setFrame(CAN_EFF_FLAG | 0x1dddb001, data, 8);
    send(_socket->fd(), frame.getFrame().get(), sizeof(can_frame), 0);
}

void CanThread::sendResRank(char rank) {
    CanFrame frame;
    char data[8];
    data[7] = rank;

    frame.setFrame(CAN_EFF_FLAG | 0x1dddb001, data, 8);
    send(_socket->fd(), frame.getFrame().get(), sizeof(can_frame), 0);
}

void CanThread::sendSweeperLevels(char rank) {
    CanFrame frame;
    char data[8] = {0};
    data[0] = 2;
    data[7] = rank;

    frame.setFrame(CAN_EFF_FLAG | 0x1dddb001, data, 8);
    send(_socket->fd(), frame.getFrame().get(), sizeof(can_frame), 0);
}



