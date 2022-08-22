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
#include <QDebug>
#include <QThread>

/**
 * @brief An implement of Camera interface, get pictures from file
 */
class DisplayImage : public QObject {
    Q_OBJECT
public:
    static DisplayImage& getInstance() {
        static DisplayImage displayImage;
        return displayImage;
    }

    void acquireImage(bool dir, ResultModel* model);
//    void setExposure(int exposure);
    void setROI(int x, int y, int weight, int height);
    void setDir();
    void clearDir();

signals:
    void sendImage(QImage image);

private:
    DisplayImage(QObject* parent = nullptr);
    QDir imageDir;
    QStringList dirList;
    int fileNum;
    QThread *my_thread;

//    AlgYolov5s* _yoloAlg;
};

#endif // DISPLAYIMAGE_H
