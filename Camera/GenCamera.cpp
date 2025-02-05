#include "GenCamera.h"

using namespace Pylon;
using namespace Basler_UniversalCameraParams;
using namespace GenApi;

static QString current_time = QDateTime::currentDateTime().toString("yyyyMMdd_hh_mm");
static bool new_folder_flag = true;
static int photo_name_expo_time = 0;
static int photo_name_gain = 0;
static int photo_name_id = 1;

#define SoftwareTrigger
// in fact,its a hardware trigger define,if you wants to use the real softwaretrigger
//  _camera->TriggerSource.SetValue(TriggerSource_Line1); needs to be TriggerSource_Software
// and _camera->ExecuteSoftwareTrigger(); needs to be added to simulate the trigger action when you want to get the image.

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
    param(ParamManage::getInstance()),
    autoExpo(new AutoExposure())
//    _yoloAlg(new AlgYolov5s())
{
    PylonInitialize();
    my_thread = new QThread();
    my_thread->start();
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
    my_thread->quit();
    my_thread->wait();
    my_thread->deleteLater();
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

/**
 * @brief Acquire an image from camera
 */
void GenCamera::acquireImage(ResultModel* model) {
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
                if (_camera->WaitForFrameTriggerReady(500, TimeoutHandling_ThrowException))
                {
                    //_camera->ExecuteSoftwareTrigger();
                    qDebug() << "start grab" <<QDateTime::currentDateTime();
                    _camera->RetrieveResult(4000, ptr, TimeoutHandling_ThrowException);
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
//        SerialPort::getInstance().ack_shoot();
        cv::Mat image(ptr->GetHeight(), ptr->GetWidth(), CV_16UC1, ptr->GetBuffer());
        if(image.empty()) { return; }

//2021.10.20 lmd
        //cv::Mat expoImage = image(cv::Rect(0, 0, 2432, 329));
        /*
         20211128 area cut
         */

        float x = (0.058+ lastt12)*arg(param.model()->paramStruct().aec.speed)*0.1; //+arg(paramManage.model()->paramStruct().capture.interval) //0.058camera delay
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

        //photo name
        std::string photoName = current_time.toStdString() + "_"
                + QString("%1").arg(photo_name_id, 5, 'g', -1, '0').toStdString()
                + "_" + "e" + std::to_string(photo_name_expo_time)
                + "_" + "g" + std::to_string(photo_name_gain)
                + "_" + "s" + std::to_string(param.model()->paramStruct().aec.speed)
                + "_" + "t" + std::to_string(param.model()->paramStruct().capture.interval)
                + "_" + "x" + std::to_string(lp)
                + "_" + "y" + std::to_string(ly)
                + "_" + "w" + std::to_string(2432-2*lp)
                + "_" + "h" + std::to_string(minmax(r1.y-r2.y,2,dly-ly));

        //<date>_<hour>_<minute>_<XXXXX(order)>_<eXXXX(expo time)>_<gXXX(gain)>_<sXX(speed)>_<tXXXX(interval)>
        //20220226_17_01_00394_e3600_g86_s0_t1000_x302_y329_w1828_h895.png

        cv::Mat target = image(cv::Rect(0, 329, 2432, 896));

        //new folder
        if (new_folder_flag) {
            QDir * folder = new QDir;
            qDebug() << current_time;
            QString dir = QString::fromStdString(param.model()->paramStruct().capture.savePath) + "/" + current_time;
            folder->mkdir(dir);
            folder->mkdir(dir + "/res/");
            folder->mkdir(dir + "/res/ylabel/");
            folder->mkdir(dir + "/raw/");
            new_folder_flag = false;
        }

        std::vector<std::pair<int, double>> detectRes;
        if(param.model()->paramStruct().capture.saveRaw) {
            std::string writeName = param.model()->paramStruct().capture.savePath + "/" + current_time.toStdString() + "/raw/";
            writeName = writeName + photoName + ".png";
            target.convertTo(target, CV_16UC3);
            target = target * 16;
            photo_name_id++;
            cv::Mat writeImage = target.clone(); // expo
            WriteImageThread* thread = new WriteImageThread(writeImage, writeName);
            thread->start();
        }
        if (param.model()->paramStruct().alg.yolo) {
//            _yoloAlg->handleImage(target, target, detectRes, photoName, param.model()->paramStruct().capture.savePath + "/" +current_time.toStdString() + "/res/ylabel/");
            model->setData(detectRes);

//            SerialPort::getInstance().ack_level();
        }

        //convert the image from cv::Mat in 16bits to QImage in 8bits for display
        cvtColor(image, image, cv::COLOR_BayerRG2RGB);
        QImage sendimage(QSize(2432, 896), QImage::Format::Format_RGB888);
        quint16* img16 = (quint16*)target.data;
        for(int i = 0; i < 2432*896*3; i++) { sendimage.bits()[i] = img16[i] >> 8; }


        emit sendImage(sendimage);



        expoImage = expoImage / 16;
        expoImage.convertTo(expoImage, CV_8UC3);
        //
        cv::Mat gray;
        cv::cvtColor(expoImage, gray, COLOR_BGR2GRAY);
        vector<float> aecRes =autoExpo->getMyNextExpTime(gray, param.model()->paramStruct().aec.speed);
        //
        photo_name_expo_time = aecRes[0]*1000;
        photo_name_gain = (int)aecRes[1];
        qDebug() <<"exptime:aaaa:"<<aecRes[0]*1000<<endl;
        setExposure(aecRes[0]*1000);
        setGain((int)aecRes[1]);
        //setExposure(2000);
        //setGain(0);
        //
        setFixWhiteBalance(aecRes[2]>0.01?false:true);
        qDebug() <<"balance:aaaa:"<<aecRes[2];
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

