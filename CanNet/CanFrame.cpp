#include "CanFrame.h"
#include <linux/can/error.h>5
/**
 * @brief Constructor of CanFrame
 */
CanFrame::CanFrame() :
        _frame(std::make_shared<struct can_frame>()) {
}

/**
 * @brief Get the frame pointer
 * @return shared pointer of frame
 */
std::shared_ptr<struct can_frame> &CanFrame::getFrame() {
    return _frame;
}

/**
 * @brief Set frame id and data
 * @param id : the CAN id
 * @param data : the data, less than 8 bytes
 * @param length : the length of data
 */
void CanFrame::setFrame(int id, const char *data, int length) {
    _frame->can_id = id;
    _frame->can_dlc = length;
    for(auto i = 0; i < length; i++)
        _frame->data[i] = data[i];
}

/**
 * @brief Check the error frame
 * @return error or not
 *      @retval 0 : error
 *      @retval 1 : no error
 */
bool CanFrame::checkError() {
    return ((_frame->can_id & CAN_ERR_FLAG) == 0);
}
