#-------------------------------------------------
#
# Project created by QtCreator 2022-08-18T09:09:52
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rsvp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    UartNet/Uart.cpp \
    JsonWork/ParamManage.cpp \
    TreeModel/ParameterModel.cpp \
    TreeModel/TreeModel.cpp \
    TreeModel/TreeItem.cpp \
    Delegates/ChooseEditor.cpp \
    Delegates/PathEditor.cpp \
    Delegates/ValueDelegate.cpp \
    Delegates/ValueEditor.cpp \
    TableModel/ResultModel.cpp \
    Camera/WriteImageThread.cpp \
    Camera/GenCamera.cpp \
    AutoExpo/AutoExpo.cpp \
    Camera/FileCamera.cpp \
    Analysis/checkresult.cpp

HEADERS += \
        mainwindow.h \
    JsonWork/JsonWork.h \
    JsonWork/ParamManage.h \
    UartNet/Uart.h \
    TreeModel/ParameterModel.h \
    TreeModel/TreeModel.h \
    TreeModel/TreeItem.h \
    Delegates/ChooseEditor.h \
    Delegates/PathEditor.h \
    Delegates/ValueDelegate.h \
    Delegates/ValueEditor.h \
    Camera/GenCamera.h \
    Camera/Camera.h \
    TableModel/ResultModel.h \
    Camera/WriteImageThread.h \
    AutoExpo/AutoExpo.h \
    Camera/FileCamera.h \
    Analysis/checkresult.h

FORMS += \
        mainwindow.ui

unix:!macx: LIBS += -L$$PWD/../../Documents/vcpkg/installed/x64-linux/lib/ -ljsoncpp

INCLUDEPATH += $$PWD/../../Documents/vcpkg/installed/x64-linux/include
DEPENDPATH += $$PWD/../../Documents/vcpkg/installed/x64-linux/include

unix:!macx: PRE_TARGETDEPS += $$PWD/../../Documents/vcpkg/installed/x64-linux/lib/libjsoncpp.a

unix:!macx: LIBS += -L$$/opt/pylon/lib/ -lpylonbase
unix:!macx: LIBS += -L$$/opt/pylon/lib/ -lgxapi
unix:!macx: LIBS += -L$$/opt/pylon/lib/ -lGCBase_gcc_v3_1_Basler_pylon
unix:!macx: LIBS += -L$$/opt/pylon/lib/ -lGenApi_gcc_v3_1_Basler_pylon

INCLUDEPATH += $$PWD/../../../../opt/pylon/include
DEPENDPATH += $$PWD/../../../../opt/pylon/include

unix:!macx: LIBS += -L$$/usr/local/lib/ -lopencv_photo
unix:!macx: LIBS += -L$$PWDusr/local/lib/ -lopencv_core
unix:!macx: LIBS +=  -lopencv_imgproc
unix:!macx: LIBS +=  -lopencv_imgcodecs

INCLUDEPATH += $$/usr/local/include/opencv4
DEPENDPATH += $$/usr/local/include/opencv4

