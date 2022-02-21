#ifndef RSVP_CANSOCKET_H
#define RSVP_CANSOCKET_H
#include <linux/can.h>
#include <vector>
/**
 * @brief The package of CAN socket
 */
class CanSocket {
public:
    explicit CanSocket(int sockFd);
    ~CanSocket();
    int fd() const;
    void bindAddress(struct sockaddr_can addr) const;
    void addFilter(std::vector<std::pair<int, int>> rules) const;
    void enableLoop(bool enable) const;
    void enableRecvOwn(bool enable) const;
private:
    int _sockFd;
};
#endif //RSVP_CANSOCKET_H
