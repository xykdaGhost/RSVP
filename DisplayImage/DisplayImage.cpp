#include "DisplayImage.h"

/**
 * @brief Constructor of FileCamera
 */
DisplayImage::DisplayImage(QObject* parent) :
        imageDir(QString::fromStdString(ParamManage::getInstance().model()->paramStruct().camera.path)),
        dirList(imageDir.entryList(QDir::Files)),
        fileNum(-1) {
            setDir();
            my_thread = new QThread();
            my_thread->start();
        }
        //_yoloAlg(new AlgYolov5s()){setDir();}

/**
 * @brief Acquire an image from camera and sen it out if needed.
 * @param dir : the direction of getting image
 *          @arg : true : next image
 *          @arg : false : previous image
 */
void DisplayImage::acquireImage(bool dir, ResultModel* model) {
    ParamManage paramManage = ParamManage::getInstance();
    if(dirList.empty())
        return;
    //the file number in the folder
    if(dir) {
        fileNum++;
        fileNum %= dirList.size();
    } else if(!dir && fileNum > 0) {
        fileNum--;
    } else if(!dir && fileNum == 0) {
        fileNum = dirList.size() - 1;
    }


    //get the particular image file name from the file list
    QString fileName = QString::fromStdString(paramManage.model()->paramStruct().camera.path) +"/raw/" + dirList[fileNum];
    //read the image
    cv::Mat image;
    quint8* img;
    image = cv::imread(fileName.toStdString(), cv::IMREAD_UNCHANGED);
    QImage sendimage(QSize(2432, 896), QImage::Format::Format_RGB888);
    //yolo part
        std::vector<std::pair<int, double>> detectRes;

//        if (GLOBAL_YOLO) { _yoloAlg->handleImage(image, detectRes, _dirList[_fileNum].left(_dirList[_fileNum].size() - 4).toStdString(), paramManage.model()->paramStruct().capture.savePath + "/res/ylabel/"); }

    //convert the image from cv::Mat in 16bits to QImage in 8bits for display
    quint16* img16 = (quint16*)image.data;
    for(int i = 0; i < 2432*896*3; i++) { sendimage.bits()[i] = img16[i] >> 8; }

    model->setData(detectRes);
    emit sendImage(sendimage);
}

/**
 * @brief Scan the address and set the read direction
 */
void DisplayImage::setDir() {
    clearDir();
    fileNum = -1;
    imageDir = QString::fromStdString(ParamManage::getInstance().model()->paramStruct().camera.path) + "/raw/";
    dirList = imageDir.entryList(QDir::Files);
}

/**
 * @brief Clear the read direction
 */
void DisplayImage::clearDir() {
    dirList.clear();
}

