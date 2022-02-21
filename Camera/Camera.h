#ifndef RSVP_CAMERA_H
#define RSVP_CAMERA_H
#include <thread>
#include <vector>
#include <QObject>
#include <QImage>
#include "../TableModel/ResultModel.h"
/**
 * @brief The interface of Camera
 */
class Camera : public QObject {
    Q_OBJECT
public:
    Camera(QObject* parent = nullptr) {};
    virtual ~Camera() = default;
    virtual void acquireImage(ResultModel* model = nullptr) = 0;
    virtual void setExposure(int exposure) = 0;
    virtual void setGain(int gain) = 0;
    virtual void setROI(int x, int y, int width, int height) = 0;
signals:
    void sendImage(QImage image);
    void sendStatus(QString status);
};

#endif //RSVP_CAMERA_H
