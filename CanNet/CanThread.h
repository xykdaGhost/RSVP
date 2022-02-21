#ifndef CANTHREAD_H
#define CANTHREAD_H
#include <QThread>
#include "CanSocket.h"
class CanThread : public QObject
{
    Q_OBJECT
public:
    static CanThread& getInstance(QString canDev = "can0") {
        static CanThread canThread(canDev);
        return canThread;
    }
private:
    CanSocket* _socket;

    QThread* _thread;

    CanThread(QString canDev, QObject* parent = nullptr);

public slots:
    void recvThread();
    void sendRes(std::vector<std::pair<int, double>> res);
    void sendResRank(char rank);
    void sendSweeperLevels(char rank);

signals:
    void speedMessage(int speed);
};

#endif // CANTHREAD_H
