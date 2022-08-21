#ifndef GENCAMERA_H
#define GENCAMERA_H

#include "Camera.h"
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>
#include <thread>
#include <mutex>
#include <QThread>
#include <QImage>

///**
// * @brief An implement of Camera interface, get pictures from Basler camera
// */
//class GenCamera : public Camera {
//public:
//    static GenCamera& getInstance() {
//        static GenCamera genCamera;
//        return genCamera;
//    }
//    ~GenCamera() override;
//    void acquireImage(ResultModel* model = nullptr) override;
//    void setExposure(int exposure) override;
//    void setGain(int gain) override;
//    void setROI(int x, int y, int width, int height) override;
//    void setFixWhiteBalance(bool flag = true);

//    void openCamera();

//private:
//    GenCamera(QObject* parent = nullptr);
//    Pylon::CBaslerUniversalInstantCamera* _camera;
//    Pylon::CGrabResultPtr _ptrGrabBuffer;
//    Pylon::CGrabResultPtr _ptrGrabResult;
//    QThread* _grabThread;
//    std::mutex _mutex;
//    AutoExposure* autoExpo;
//    AlgYolov5s* _yoloAlg;

//    void grabbingPics();
//    QString getRandomString(int length);
//    float lastt12=0;
//    cv::Mat M_1,M;
//};

#endif // GENCAMERA_H
