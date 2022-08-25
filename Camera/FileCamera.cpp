#include "FileCamera.h"

/**
 * @brief Constructor of FileCamera
 */
FileCamera::FileCamera(QObject* parent) :
        Camera(parent),
        _dir(QString::fromStdString(ParamManage::getInstance().model()->paramStruct().camera.path)),
        _dirList(_dir.entryList(QDir::Files)),
        _fileNum(-1),
        paramManage(ParamManage::getInstance())
//        ,_yoloAlg(new AlgYolov5s())
{setDir();}

/**
 * @brief Destructor of FileCamera
 */
FileCamera::~FileCamera() {}

/**
 * @brief Acquire an image from camera and sen it out if needed.
 * @param dir : the direction of getting image
 *          @arg : true : next image
 *          @arg : false : previous image
 */
void FileCamera::acquireImage(bool dir, ResultModel* model) {
    if(_dirList.empty())
        return;
    //the file number in the folder
    QString FileName;
    if(dir) {
        _fileNum++;
        _fileNum %= _dirList.size();
    } else if(!dir && _fileNum > 0) {
        _fileNum--;
    } else if(!dir && _fileNum == 0) {
        _fileNum = _dirList.size() - 1;
    }
   //get the particular image file name from the file list
    FileName = QString::fromStdString(paramManage.model()->paramStruct().camera.path) +"/raw/" + _dirList[_fileNum];
    //read the image
    cv::Mat image;
    cv::Mat imageYolo = cv::imread(FileName.toStdString());
    image = cv::imread(FileName.toStdString(),cv::IMREAD_UNCHANGED);
    QImage sendimage(QSize(2432, 896), QImage::Format::Format_RGB888);

    std::vector<std::pair<int, double>> detectRes;
//    if (paramManage.model()->paramStruct().alg.yolo) { _yoloAlg->handleImage(imageYolo, image, detectRes, _dirList[_fileNum].left(_dirList[_fileNum].size() - 4).toStdString(), paramManage.model()->paramStruct().capture.savePath + "/res/ylabel/"); }
    cvtColor(image, image, cv::COLOR_BGR2RGB);
    //convert the image from cv::Mat in 16bits to QImage in 8bits for display
    quint16* img16 = (quint16*)image.data;
    for(int i = 0; i < 2432*896*3; i++) { sendimage.bits()[i] = img16[i] >> 8; }
    model->setData(detectRes);

    emit sendImage(sendimage);
}

/**
 * @brief Scan the address and set the read direction
 */
void FileCamera::setDir() {
    clearDir();
    _fileNum = -1;
    _dir = QString::fromStdString(ParamManage::getInstance().model()->paramStruct().camera.path) + "/raw/";
    _dirList = _dir.entryList(QDir::Files);
}

/**
 * @brief Clear the read direction
 */
void FileCamera::clearDir() {
    _dirList.clear();
}
