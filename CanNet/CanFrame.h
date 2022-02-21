#ifndef RSVP_CANFRAME_H
#define RSVP_CANFRAME_H
#include <linux/can.h>
#include <memory>
/**
 * @brief The package of can frame
 */
class CanFrame {
public:
    CanFrame();
    ~CanFrame() = default;
    std::shared_ptr<struct can_frame>& getFrame();
    void setFrame(int id,  const char* data, int length);
    bool checkError();
private:
    std::shared_ptr<struct can_frame> _frame;
};

#endif //RSVP_CANFRAME_H
