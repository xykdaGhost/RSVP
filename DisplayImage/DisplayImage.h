#ifndef DISPLAYIMAGE_H
#define DISPLAYIMAGE_H

#include <sys/eventfd.h>
#include "../Camera/Camera.h"
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QTableWidget>
#include <iostream>
#include <algorithm>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include "../JsonWork/ParamManage.h"
#include <dirent.h>
#include <ctime>
#include <sys/stat.h>
#include <QDebug>
#include <vector>
#include "../TableModel/ResultModel.h"

/**
 * @brief An implement of Camera interface, get pictures from file
 */
class DisplayImage {
    Q_OBJECT
public:
    static DisplayImage& getInstance() {
        static DisplayImage displayImage;
        return displayImage;
    }
    ~DisplayImage() override;
    void acquireImage(ResultModel* model = nullptr) override {};
//    void setExposure(int exposure) override {};
//    void setGain(int gain) override {};
    void setROI(int x, int y, int weight, int height) override {};
    void setDir();
    void clearDir();
    void acquireImage(bool dir, QTableWidget* widget = nullptr, ResultModel* model = nullptr);

private:
    FileCamera(QObject* parent = nullptr);
    QDir imageDir;
    QStringList dirList;
    int fileNum;

//    AlgYolov5s* _yoloAlg;
};

#endif // DISPLAYIMAGE_H
