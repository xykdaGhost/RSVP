#ifndef RSVP_FILECAMERA_H
#define RSVP_FILECAMERA_H
#include <sys/eventfd.h>
#include "Camera.h"
#include <QStringList>
#include <QDir>
#include <QImage>
#include <QTableWidget>
#include "../../yolo/RsvpAlgorithm/AlgYolov5/AlgYolov5.h"

/**
 * @brief An implement of Camera interface, get pictures from file
 */
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
    void acquireImage(bool dir, QTableWidget* widget = nullptr, ResultModel* model = nullptr);

private:
    FileCamera(QObject* parent = nullptr);
    QDir _dir;
    QStringList _dirList;
    int _fileNum;

    AlgYolov5s* _yoloAlg;
};




#endif //RSVP_FILECAMERA_H
