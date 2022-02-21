#include "CanSocket.h"
#include <sys/socket.h>
#include <linux/can/raw.h>
#include <cstdio>
#include <unistd.h>
/**
 * @brief Constructor of CanSocket
 * @param sockFd : the can socket file descriptor
 */
CanSocket::CanSocket(int sockFd) :
        _sockFd(sockFd) {
}

/**
 * @brief Destructor of CanSocket
 */
CanSocket::~CanSocket() {
    close(_sockFd);
}

/**
 * @brief Get the file descriptor
 * @return the file descriptor
 */
int CanSocket::fd() const {
    return _sockFd;
}

/**
 * @brief Bind the CAN device to file descriptor
 * @param addr : the can addr struct
 */
void CanSocket::bindAddress(struct sockaddr_can addr) const {
    int ret = ::bind(_sockFd, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0)
        perror("can bind error");
}

/**
 * @brief Add filter rule
 * @param rules : the filter rules
 */
void CanSocket::addFilter(std::vector<std::pair<int, int>> rules) const {
    struct can_filter filter[rules.size()];
    for(auto i = 0; i < rules.size(); i++) {
        filter[i].can_id = rules[i].first;
        filter[i].can_mask = rules[i].second;
    }
    setsockopt(_sockFd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
}

/**
 * @brief Enable the loopback mode
 * @param enable : 1 for enable, 0 for disable
 */
void CanSocket::enableLoop(bool enable) const {
    int loop = enable;
    setsockopt(_sockFd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loop, sizeof(loop));
}

/**
 * @brief Enable receive own message. If disabled, the device
 * will not receive message they send.
 * @param enable : 1 for enable, 0 for disable
 */
void CanSocket::enableRecvOwn(bool enable) const {
    int recvOwn = enable;
    setsockopt(_sockFd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recvOwn, sizeof(recvOwn));
}

