#include "GenCamera.h"
#include "../JsonWork/ParamManage.h"
#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/core.hpp>
#include <QDebug>
#include <QDateTime>
#include <chrono>
#include "WriteImageThread.h"
#include "../CanNet/CanThread.h"
#include <QDateTime>
#include <QDir>
using namespace Pylon;
using namespace Basler_UniversalCameraParams;
using namespace GenApi;

#define SoftwareTrigger
//#define FixedWhiteBalance
static QString current_time = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm");
static bool new_folder_flag = true;
static int photo_name_expo_time = 0;
static int photo_name_gain = 0;
static int photo_name_id = 1;
static int speed = 0;

extern int GLOBAL_YOLO;
extern int GLOBAL_SPEED;
extern int GLOBAL_SAVEPICTURE;

inline cv::Point getTargetPoint(cv::Point pt_origin, cv::Mat warpMatrix) {
    cv::Mat_<double> mat_pt(3, 1);
    mat_pt(0, 0) = pt_origin.x;
    mat_pt(1, 0) = pt_origin.y;
    mat_pt(2, 0) = 1;
    cv::Mat mat_pt_view = warpMatrix * mat_pt;
    double a1 = mat_pt_view.at<double>(0, 0);
    double a2 = mat_pt_view.at<double>(1, 0);
    double a3 = mat_pt_view.at<double>(2, 0);
    return cv::Point(a1 * 1.0 / a3, a2 * 1.0 / a3);
}

inline int minmax(int pixel,int minv,int maxv) {
        pixel = pixel > maxv ? maxv : pixel;
        pixel = pixel < minv ? minv : pixel;
        return pixel;
    }
/**
 * @brief Constructor of GenCamera
 */
GenCamera::GenCamera(QObject* parent) :
    Camera(parent),
    autoExpo(new AutoExposure()),
    _yoloAlg(new AlgYolov5s()){
    PylonInitialize();

}

void GenCamera::openCamera() {
    try {
        _camera = new CBaslerUniversalInstantCamera(CTlFactory::GetInstance().CreateFirstDevice());
        _camera->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
        _camera->Open();

        // Get camera device information.
        qDebug() << "Camera Device Information" << endl
             << "=========================" << endl;
        qDebug() << "Vendor           : "
             << _camera->DeviceVendorName.GetValue() << endl;
        qDebug() << "Model            : "
             << _camera->DeviceModelName.GetValue() << endl;
        qDebug() << "Firmware version : "
             << _camera->DeviceFirmwareVersion.GetValue() << endl;
        qDebug() << "Format : "
             << _camera->PixelFormat.ToString() << endl;
        // Camera settings.
        qDebug() << "Camera Device Settings" << endl
             << "======================" << endl;
        _camera->MaxNumBuffer = 5;
//2021.12.28 lmd
#ifdef SoftwareTrigger
    _camera->RegisterConfiguration(new CSoftwareTriggerConfiguration, RegistrationMode_ReplaceAll, Cleanup_Delete);
    _camera->TriggerMode.SetValue(TriggerMode_On); //TriggerMode_Off TriggerMode_OnŽ¥·¢Ä£Êœ TriggerModeEnums
    _camera->TriggerSource.SetValue(TriggerSource_Line1); // TriggerSource_Line1 TriggerSourceEnums TriggerSource_Software
#else
    _camera->TriggerMode.SetValue(TriggerMode_Off); //TriggerMode_Off TriggerMode_OnŽ¥·¢Ä£Êœ TriggerModeEnums
    _camera->TriggerSource.SetValue(TriggerSource_Line1); // TriggerSource_Line1 TriggerSourceEnums TriggerSource_Software
#endif

    _camera->LightSourceSelector.SetValue(LightSourceSelector_Daylight); //LightSourceSelector_Daylight  LightSourceSelector_Daylight6500K
#ifdef FixedWhiteBalance
    //5000k red 1.578125 green 1.0 blue 2.28125 ¡Ì
    //6500k red 1.828125 green 1.0 blue 1.9375
    _camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
    _camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Red);
    _camera->BalanceRatioAbs.SetValue(1.578125);
    //// Set the green intensity to 100%
    _camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Green);
    _camera->BalanceRatio.SetValue(1);
    //// Set the blue intensity to 219.678%
    _camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
    _camera->BalanceRatio.SetValue(2.0);//2.28125
#else
    _camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);// BalanceWhiteAuto_Continuous Once
#endif

#ifdef SoftwareTrigger
    _camera->StartGrabbing(GrabStrategy_LatestImageOnly);
#else
    _camera->StartGrabbing();
    _grabThread = new QThread(this);
    this->moveToThread(_grabThread);
    connect(_grabThread, &QThread::started, this, &GenCamera::grabbingPics);
    _grabThread->start();
#endif

        vector<double> Mdata{ 0.46464507552316109,0.31322056430355943,-384.00841183616387,0.0000000000000000,-0.91177886268307751,1117.8408856494530,-2.5774733953409634e-19,0.0017305003552682847,1.0000000000000000 };
        M.create(3, 3, CV_64FC1);
        for (int i = 0; i < 3; ++i) {
            double* data = M.ptr<double>(i);
            for (int j = 0; j < 3; ++j) {
                data[j] = Mdata[i * 3 + j];
            }
        }
        //r = getTargetPoint(cv::Point(3.62, 0), M_1); m±äÍŒÏñ
        cv::invert(M, M_1);

        emit sendStatus("已连接");
    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
        emit sendStatus("未连接");
    }
}

/**
 * @brief Destructor of GenCamera
 */
GenCamera::~GenCamera() {
    try {
        if(_camera != nullptr)
            _camera->Close();
        PylonTerminate();
    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
    }

}

/**
 * @brief Acquire an image from camera
 */
void GenCamera::acquireImage(ResultModel* model) {
    ParamManage& paramManage = ParamManage::getInstance();
    //auto t1_e = std::chrono::steady_clock::now(); //auto_exp
    try {
        CGrabResultPtr ptr;
        auto t1 = std::chrono::steady_clock::now();
#ifdef SoftwareTrigger
    if (_camera->IsGrabbing())
        {
            qDebug() << "start wait" <<QDateTime::currentDateTime();
            if (_camera->CanWaitForFrameTriggerReady())
            {

                // Execute the software trigger. Wait up to 1000 ms for the camera to be ready for trigger.
                if (_camera->WaitForFrameTriggerReady(1000, TimeoutHandling_ThrowException))
                {
                    _camera->ExecuteSoftwareTrigger();
                    qDebug() << "start grab" <<QDateTime::currentDateTime();
                    _camera->RetrieveResult(10000, ptr, TimeoutHandling_ThrowException);
                    if (ptr->GrabSucceeded()) {

                        qDebug() << "grab success" << QDateTime::currentDateTime();
                        //cv::Mat image(ptr->GetHeight(), ptr->GetWidth(), CV_16UC1, _ptrGrabBuffer->GetBuffer());
                        //cvtColor(image, image, cv::COLOR_BayerRG2RGB);
                    }
                    else {
                        cout << "Error: " << ptr->GetErrorCode() << " " << _ptrGrabBuffer->GetErrorDescription() << endl; //·Çµ÷ÊÔœ×¶ÎÊ¹ÓÃ×îºÃ×¢ÊÍµô
                    }

                }
            }
        }
#else
        {
            std::unique_lock<std::mutex> lock(_mutex);
            std::swap(ptr, _ptrGrabResult);
        }

        //check the image validation
        if(!ptr.IsValid())
            return;
#endif


        //create a Mat image from buffer
        SerialPort::getInstance().ack_shoot();
        cv::Mat image(ptr->GetHeight(), ptr->GetWidth(), CV_16UC1, ptr->GetBuffer());
        qDebug() << "buffer" << QDateTime::currentDateTime();
        if(image.empty())
            return;

        //save photo
        cv::Mat target_raw = image(cv::Rect(0, 329, 2432, 896));


        //convert the image from bayer to rgb
        cvtColor(image, image, cv::COLOR_BayerRG2RGB);
//        time_t now = time(0);
//        //get the time point and format it as the file name
//        tm *ltm = localtime(&now);
//        std::string timeStamp = std::to_string(ltm->tm_mon) + "_" +
//                std::to_string(ltm->tm_mday) + "_" +
//                getRandomString(5).toStdString();


        //set photo name
        speed = paramManage.model()->paramStruct().aec.speed;



//2021.10.20 lmd
        //cv::Mat expoImage = image(cv::Rect(0, 0, 2432, 329));
        /*
         20211128 area cut
         */
        qDebug() << "now speed is : " << GLOBAL_SPEED;
        float x = (0.058+ lastt12)*arg(GLOBAL_SPEED)*0.1; //+arg(paramManage.model()->paramStruct().capture.interval) //0.058camera delay
        cv::Point r2 = getTargetPoint(cv::Point(0, x+520), M_1);//×óÉÏ
        cv::Point ur = getTargetPoint(cv::Point(362, x+520), M_1);
        cv::Point dr = getTargetPoint(cv::Point(362, x), M_1);
        cv::Point r1 = getTargetPoint(cv::Point(0,x), M_1); //×óÏÂ

        int lp = (r2.x - r1.x) / 2.0 + r1.x;
        lp = minmax(lp,1,1215);
        int ly = minmax(r2.y, 1, 1224);
        int dly = minmax(r1.y, ly, 1224);
        cv::Rect area1(lp,ly, 2432-2*lp, minmax(r1.y-r2.y,2,dly-ly));
        std::cout<<"x:"<<lp<<",y:"<<ly<<",width:"<<2432-2*lp<<",height:"<<minmax(r1.y-r2.y,2,dly-ly)<<std::endl;

        cv::Mat expoImage = image(area1);

        //

        std::string photoName = current_time.toStdString() + "_"
                + QString("%1").arg(photo_name_id, 5, 'g', -1, '0').toStdString()
                + "_" + QString("e%1").arg(photo_name_expo_time, 5,10).toStdString()
                + "_" + QString("g%1").arg(photo_name_gain, 5,10).toStdString()
                + "_" + QString("s%1").arg(GLOBAL_SPEED, 3,10).toStdString()
                + "_" + QString("t%1").arg(paramManage.model()->paramStruct().capture.saveRawInterval).toStdString()
                + "_" +QString("x%1").arg(lp,4,10).toStdString()
                + "_" +QString("y%1").arg(ly,4,10).toStdString()
                + "_" +QString("w%1").arg(2432-2*lp, 4,10).toStdString()
                + "_" +QString("h%1").arg(minmax(r1.y-r2.y,2,dly-ly),4,10).toStdString();



        cv::Mat target = image(cv::Rect(0, 329, 2432, 896));

        //new folder
        if (new_folder_flag) {
            QDir * folder = new QDir;
            qDebug() << current_time;
            QString dir = QString::fromStdString(paramManage.model()->paramStruct().capture.savePath) + "/" + current_time;
            folder->mkdir(dir);
            folder->mkdir(dir + "/res/");
            folder->mkdir(dir + "/res/ylabel/");
            folder->mkdir(dir + "/raw/");

            new_folder_flag = false;
        }

        if(GLOBAL_SAVEPICTURE) {
//            target.convertTo(target, CV_8UC1);
            std::string writeName = paramManage.model()->paramStruct().capture.savePath + "/" + current_time.toStdString() + "/raw/";
            writeName = writeName + photoName + ".png";
            target_raw = image * 16; //target*16
            target_raw.convertTo(target_raw, CV_16UC3);
            photo_name_id++;
            cv::Mat writeImage = target_raw.clone(); // expo

            WriteImageThread* thread = new WriteImageThread(writeImage, writeName);
            thread->start();
        }

        std::vector<std::pair<int, double>> detectRes;

        if (GLOBAL_YOLO) {
        _yoloAlg->handleImage(target, detectRes, photoName, paramManage.model()->paramStruct().capture.savePath + "/" +current_time.toStdString() +
                              "/res/ylabel/");}

        //convert the image from bayer to rgb
        cvtColor(image, image, cv::COLOR_BGR2RGB);
        //convert the image from cv::Mat in 16bits to QImage in 8bits for display
        QImage sendimage(QSize(2432, 896), QImage::Format::Format_RGB888);
        quint16* img16 = (quint16*)target.data;
        for(int i = 0; i < 2432*896*3; i++) {
            sendimage.bits()[i] = img16[i] >> 4;
        }


        if (GLOBAL_YOLO) {
            model->setData(detectRes);

            SerialPort::getInstance().ack_level();

            CanThread::getInstance().sendRes(detectRes);
        }

        emit sendImage(sendimage);


        paramManage.updateJsonRoot();
        expoImage = expoImage / 16;
        expoImage.convertTo(expoImage, CV_8UC3);
        //vector<float> aecRes = autoExpo->getNextExpTime(expoImage, paramManage.model()->paramStruct().aec.speed);
        //
        cv::Mat gray;
        cv::cvtColor(expoImage, gray, COLOR_RGB2GRAY);
        vector<float> aecRes =autoExpo->getMyNextExpTime(gray, paramManage.model()->paramStruct().aec.speed);
        //
        photo_name_expo_time = aecRes[0]*1000;
        photo_name_gain = (int)aecRes[1];
        cout<<"exptime:aaaa:"<<aecRes[0]*1000<<endl;
        setExposure(aecRes[0]*1000);
        setGain((int)aecRes[1]);
        //
        setFixWhiteBalance(aecRes[2]>0.01?false:true);
        //

        auto t2 = std::chrono::steady_clock::now();
        auto t = t2 - t1;
        lastt12 = (float)t.count();
        std::cout<<"time all:"<<std::chrono::duration<double>(t).count()<<"s"<<std::endl;

    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
    }

}

/**
 * @brief Set the exposure parameter of camera
 * @param exposure : the exposure parameter value
 */
void GenCamera::setExposure(int exposure) {
    if(_camera == nullptr)
        return;
    try {
        _camera->ExposureTimeRaw.SetValue(exposure);
        qDebug() << "Exposure updated:" << _camera->ExposureTimeRaw.GetValue() << endl;
    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
    }
}

/**
 * @brief Set the gain parameter of camera
 * @param gain : the gain parameter value
 */
void GenCamera::setGain(int gain) {
    if(_camera == nullptr)
        return;
    try {
        _camera->GainRaw.SetValue(gain);
        qDebug() << "Gain updated:" << _camera->GainRaw.GetValue() << endl;
    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
    }

}
//2022.1.11 lmd
void GenCamera::setFixWhiteBalance(bool flag) {
    if (_camera == nullptr)
        return;
    if (flag) { //fixed
        _camera->LightSourceSelector.SetValue(LightSourceSelector_Daylight);
        _camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Off);
        _camera->BalanceRatioSelector.SetValue(BalanceRatioSelector_Blue);
        _camera->BalanceRatio.SetValue(1.5);//2.28125
    }
    //auto
    else {
        _camera->BalanceWhiteAuto.SetValue(BalanceWhiteAuto_Continuous);
    }
    cout << "WhiteBalance updated:" << ((flag == true) ? "Fixed":"Auto") << endl;
}


/**
 * @brief Set the ROT parameters of camera
 * @param x : the x position
 * @param y : the y position
 * @param width : the width of image
 * @param height : the height of image
 */
void GenCamera::setROI(int x, int y, int width, int height) {
    if(_camera == nullptr)
        return;
    try {
        _camera->OffsetX.SetValue(x);
        _camera->OffsetY.SetValue(y);
        _camera->Width.SetValue(width);
        _camera->Height.SetValue(height);

        qDebug() << "ROI updated:" << _camera->OffsetX.GetValue() << " "
                                << _camera->OffsetY.GetValue() << " "
                                << _camera->Width.GetValue() << " "
                                << _camera->Height.GetValue() << endl;
    }  catch (const GenericException& e) {
        // Error handling.
        std::cerr << "An exception occurred." << std::endl
            << e.GetDescription() << std::endl;
    }

}

/**
 * @brief Grab picture thread handler
 */
void GenCamera::grabbingPics() {
    while(1) {
        try {
            if(_camera->IsGrabbing()) {
                _camera->RetrieveResult(100000, _ptrGrabBuffer, TimeoutHandling_Return);
                if(_ptrGrabBuffer->GrabSucceeded()) {
                    std::unique_lock<std::mutex> lock(_mutex);
                    std::swap(_ptrGrabBuffer, _ptrGrabResult);
                }
            }
        }  catch (const GenericException& e) {
            // Error handling.
            std::cerr << "An exception occurred." << std::endl
                << e.GetDescription() << std::endl;
        }
    }
}

QString GenCamera::getRandomString(int length)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());

    const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int chrs_size = sizeof(chrs);

    char* ch = new char[length + 1];
    memset(ch, 0, length + 1);
    int randomx = 0;
    for (int i = 0; i < length; ++i)
    {
        randomx= rand() % (chrs_size - 1);
        ch[i] = chrs[randomx];
    }

    QString ret(ch);
    delete[] ch;
    return ret;
}


