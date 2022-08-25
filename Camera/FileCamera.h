#ifndef FILECAMERA_H
#define FILECAMERA_H
#include <sys/eventfd.h>
#include "Camera.h"
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QTableWidget>
//#include "../../yolo/RsvpAlgorithm/AlgYolov5/AlgYolov5.h"
#include <iostream>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "../JsonWork/ParamManage.h"
#include <dirent.h>
#include <ctime>
#include <sys/stat.h>
#include <QDebug>
//#include <Analysis/checkresult.h>
#include <vector>


class FileCamera : public Camera {
    Q_OBJECT
public:
    static FileCamera& getInstance() {
        static FileCamera fileCamera;
        return fileCamera;
    }
    ~FileCamera() override;
    void acquireImage(ResultModel* model = nullptr) override {};
    void setExposure(int exposure) override {};
    void setGain(int gain) override {};
    void setROI(int x, int y, int weight, int height) override {};

    void setDir();
    void clearDir();
    void acquireImage(bool dir, ResultModel* model = nullptr);

private:
    FileCamera(QObject* parent = nullptr);
    QDir _dir;
    QStringList _dirList;
    int _fileNum;
    ParamManage paramManage;
//    AlgYolov5s* _yoloAlg;
};

#endif // FILECAMERA_H
