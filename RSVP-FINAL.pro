QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS+= -fopenmp

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Analysis/checkresult.cpp \
    AutoExpo/AutoExpo.cpp \
    Delegates/ChooseEditor.cpp \
    Delegates/PathEditor.cpp \
    Delegates/ValueDelegate.cpp \
    Delegates/ValueEditor.cpp \
    JsonWork/ParamManage.cpp \
    TableModel/ResultModel.cpp \
    TreeModel/ParameterModel.cpp \
    TreeModel/TreeItem.cpp \
    TreeModel/TreeModel.cpp \
    main.cpp \
    mainwindow.cpp \
    Camera/FileCamera.cpp \
    Camera/GenCamera.cpp \
    Camera/WriteImageThread.cpp \
    CanNet/CanFrame.cpp \
    CanNet/CanSocket.cpp \
    CanNet/CanThread.cpp \
    TableModel/StatusModel.cpp \
    UartNet/SerialPort.cpp \

HEADERS += \
    Analysis/checkresult.h \
    AutoExpo/AutoExpo.h \
    Camera/Camera.h \
    Camera/FileCamera.h \
    Camera/GenCamera.h \
    Delegates/ChooseEditor.h \
    Delegates/PathEditor.h \
    Delegates/ValueDelegate.h \
    Delegates/ValueEditor.h \
    JsonWork/JsonWork.h \
    JsonWork/ParamManage.h \
    TableModel/ResultModel.h \
    TreeModel/ParameterModel.h \
    TreeModel/TreeItem.h \
    TreeModel/TreeModel.h \
    mainwindow.h \
    CanNet/CanThread.h \
    CanNet/CanFrame.h \
    CanNet/CanSocket.h \
    Camera/WriteImageThread.h \
    TableModel/StatusModel.h \
    UartNet/SerialPort.h \

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

unix:!macx: LIBS += -L$$PWD/../../../../usr/lib/x86_64-linux-gnu/ -ljsoncpp

INCLUDEPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../usr/lib/x86_64-linux-gnu

unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_core
#unix:!macx: LIBS += -L$$PWD/../../../../usr/local/lib/ -lopencv_photo
unix:!macx: LIBS +=  -lopencv_imgproc
unix:!macx: LIBS +=  -lopencv_imgcodecs


INCLUDEPATH += $$PWD/../../../../usr/local/include/opencv4/
DEPENDPATH += $$PWD/../../../../usr/local/include/opencv4/


#unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lAlgRoadSeg

#INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
#DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm

#unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lAlgYolov5

#INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
#DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm

#unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lmyplugins

#INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
#DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm

unix:!macx: LIBS += -L$$PWD/../../../../usr/local/cuda/targets/aarch64-linux/lib/ -lcudart

INCLUDEPATH += $$PWD/../../../../usr/local/cuda/targets/aarch64-linux/include
DEPENDPATH += $$PWD/../../../../usr/local/cuda/targets/aarch64-linux/include

unix:!macx: LIBS += -L$$PWD/../yolo/source/cmake-build-debug/ -lmyplugins

unix:!macx: LIBS += -L$$PWD/../../../../opt/pylon/lib/ -lpylonbase
unix:!macx: LIBS += -L$$PWD/../../../../opt/pylon/lib/ -lgxapi
unix:!macx: LIBS += -L$$PWD/../../../../opt/pylon/lib/ -lGCBase_gcc_v3_1_Basler_pylon
unix:!macx: LIBS += -L$$PWD/../../../../opt/pylon/lib/ -lGenApi_gcc_v3_1_Basler_pylon

INCLUDEPATH += $$PWD/../../../../opt/pylon/include
DEPENDPATH += $$PWD/../../../../opt/pylon/include

DISTFILES +=

RESOURCES += \
    Sources/images.qrc








unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lAlgRoadSeg

INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm

unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lAlgYolov5

INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm

unix:!macx: LIBS += -L$$PWD/../yolo/RsvpAlgorithm/lib/ -lmyplugins

INCLUDEPATH += $$PWD/../yolo/RsvpAlgorithm
DEPENDPATH += $$PWD/../yolo/RsvpAlgorithm
