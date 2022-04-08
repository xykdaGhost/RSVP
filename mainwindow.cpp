

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "JsonWork/ParamManage.h"
#include "Delegates/ValueDelegate.h"
#include "TableModel/ResultModel.h"
#include "Camera/FileCamera.h"
#include "Camera/GenCamera.h"
#include "TableModel/StatusModel.h"
#include <QDateTime>
#include <QStorageInfo>
#include <QProcess>

int GLOBAL_SPEED = 5;
char canMessage = 23;
int GLOBAL_MODE = 1;
int GLOBAL_YOLO = 1;
int GLOBAL_TRASH_AMOUNT = 0;
int GLOBAL_TRASH_DENSITY = 0;
int GLOBAL_SAVEPICTURE = 1;
int GLOBAL_STORAGE = 0;


/**
 * @brief Constructor of MainWindow,
 * @param parent : the parent widget
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      _workMode(WORK_MODE::WORK),
      ui(new Ui::MainWindow),
      _analysis(false),
      _canThread(&CanThread::getInstance("can0")),
      _serialPort(&SerialPort::getInstance())
{
    ui->setupUi(this);

    //user codes from here
    static int data_time_flag = 1;
    static QString current_time;

    QStorageInfo storage= QStorageInfo::mountedVolumes().at(3);
    storage.refresh();

    connect(ui->testButton, &QPushButton::clicked, this, [=] {
        qDebug() << "send button" << QDateTime::currentDateTime();

        qDebug() << storage.rootPath();
        if (storage.isReadOnly()) {
            qDebug() << "isReadOnly:" << storage.isReadOnly();
        }
        qDebug() << "name" << storage.name();
        qDebug() << "fileSystemType:" << storage.fileSystemType();
        qDebug() << "size:" << storage.bytesTotal()/1000/1000/1000 << "GB";
        QString text = tr("%1").arg(storage.bytesAvailable()/1000/1000/1000) + "GB";

        GLOBAL_STORAGE = storage.bytesAvailable()/1000/1000/1000;

        qDebug() << "availableSize:" << text;
        _serialPort->ask_shoot();
        ui->sizeLabel->setText(text);

        _serialPort->ack_status();
    });


    connect(&SerialPort::getInstance(), SIGNAL(receive_data(QByteArray)), this, SLOT(on_receive(QByteArray)), Qt::QueuedConnection);

    _serialPort->ask_date();

    if (data_time_flag == 1) {
        data_time_flag = 2;
        current_time = QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm");
    }

    ParamManage param = ParamManage::getInstance();

    StatusModel* statusModel = new StatusModel(this);
    ui->statusView->setModel(statusModel);
    ui->statusView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->statusView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->statusView->horizontalHeader()->setHidden(true);
    ui->statusView->setAlternatingRowColors(true);

    connect(&GenCamera::getInstance(), &GenCamera::sendStatus, statusModel, &StatusModel::setCamera);
    connect(_canThread, &CanThread::speedMessage, statusModel, &StatusModel::setSpeed);

    GenCamera::getInstance().openCamera();

    //setup parameter model, view and delegate
    ValueDelegate* delegate = new ValueDelegate(this);

    ui->parameterView->setModel(param.model());
    ui->parameterView->expandAll();
    ui->parameterView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->parameterView->setItemDelegate(delegate);
    ui->parameterView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    //setup result model and view
    ResultModel* result = new ResultModel(this);

    ui->resultView->setFont(QFont("", 16));
    ui->resultView->setModel(result);
    ui->resultView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultView->setAlternatingRowColors(true);


    //setup mode button
    ui->workModeButton->setEnabled(false);

    ui->analysisWidget->setFont(QFont("", 14));
    ui->analysisWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->analysisWidget->horizontalHeader()->setFont(QFont("", 18));
    ui->analysisWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->analysisWidget->verticalHeader()->setFont(QFont("", 18));
    ui->analysisWidget->setAlternatingRowColors(true);


    //connect button signals
    connect(ui->saveButton, &QPushButton::clicked, this, [=] {
        //on save action, update the Json root and parameter struct, write to file and enable the new parameter
        ParamManage::getInstance().updateJsonRoot();
        ParamManage::getInstance().writeJsonToFile("settings.json");
        updateParameter();
    });

    connect(ui->applyButton, &QPushButton::clicked, this, [=] {
        //on apply action, update the Json root and parameter struct and enable the new parameter
        ParamManage::getInstance().updateJsonRoot();
        updateParameter();
    });

    //connect the sendImage signal and display the image in the widow
    connect(&FileCamera::getInstance(), &FileCamera::sendImage, [=] (QImage image) {
        if(ui->pictureLabel->width() / ui->pictureLabel->height() > image.width() / image.height())
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(image.width() * ui->pictureLabel->height() / image.height(), ui->pictureLabel->height())));
        else
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->pictureLabel->width(), image.height() * ui->pictureLabel->width() / image.width())));
    });
    connect(&GenCamera::getInstance(), &GenCamera::sendImage, [=] (QImage image) {
        if(ui->pictureLabel->width() / ui->pictureLabel->height() > image.width() / image.height())
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(image.width() * ui->pictureLabel->height() / image.height(), ui->pictureLabel->height())));
        else
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->pictureLabel->width(), image.height() * ui->pictureLabel->width() / image.width())));
    });

    connect(ui->tabWidget, &QTabWidget::currentChanged, [=] (int tab) {
        _analysis = (tab == 1);
    });

    //acquireImage from different camera by work mode
    connect(ui->nextButton, &QPushButton::clicked, this, [=] {
        if(_workMode == WORK_MODE::SHOW)
            FileCamera::getInstance().acquireImage(true, _analysis? ui->analysisWidget : nullptr, result);
        else
            GenCamera::getInstance().acquireImage(result);
    });
    connect(ui->previousButton, &QPushButton::clicked, this, [=] {
        if(_workMode == WORK_MODE::SHOW)
            FileCamera::getInstance().acquireImage(false, _analysis? ui->analysisWidget : nullptr, result);
    });
    //set timer event, acquire image on every timer period
    connect(ui->playButton, &QPushButton::clicked, this, [=] {
        if(ui->playButton->text() == "启动") {
            ui->playButton->setText("停止");
            ui->nextButton->setEnabled(false);
            ui->previousButton->setEnabled(false);
            _timerId = startTimer(500);
            CanThread::getInstance().sendResRank(canMessage);
        } else {
            ui->playButton->setText("启动");
            ui->nextButton->setEnabled(true);
            ui->previousButton->setEnabled(true);
            killTimer(_timerId);
        }
    });

    //change work mode
    connect(ui->workModeButton, &QPushButton::clicked, this, [=] {
        _workMode = WORK_MODE::WORK;
        ui->workModeButton->setEnabled(false);
        ui->showModeButton->setEnabled(true);
        ui->debugModeButton->setEnabled(true);
        ui->sampleMdoeButton->setEnabled(true);
    });
    connect(ui->showModeButton, &QPushButton::clicked, this, [=] {
        _workMode = WORK_MODE::SHOW;
        ui->workModeButton->setEnabled(true);
        ui->showModeButton->setEnabled(false);
        ui->debugModeButton->setEnabled(true);
        ui->sampleMdoeButton->setEnabled(true);
    });
    connect(ui->debugModeButton, &QPushButton::clicked, this, [=] {
        _workMode = WORK_MODE::DEBUG;
        ui->workModeButton->setEnabled(true);
        ui->showModeButton->setEnabled(true);
        ui->debugModeButton->setEnabled(false);
        ui->sampleMdoeButton->setEnabled(true);
    });
    connect(ui->sampleMdoeButton, &QPushButton::clicked, this, [=] {
        _workMode = WORK_MODE::SAMPLE;
        ui->workModeButton->setEnabled(true);
        ui->showModeButton->setEnabled(true);
        ui->debugModeButton->setEnabled(true);
        ui->sampleMdoeButton->setEnabled(false);
    });

    connect(ui->closeButton, &QPushButton::clicked, this, [=] {this->close();});

    connect(ui->tdstButton, &QPushButton::clicked, this, [=] {CanThread::getInstance().sendSweeperLevels(0);});
    connect(ui->thstButton, &QPushButton::clicked, this, [=] {CanThread::getInstance().sendSweeperLevels(1);});
    connect(ui->tfstButton, &QPushButton::clicked, this, [=] {CanThread::getInstance().sendSweeperLevels(2);});
    connect(ui->tgstButton, &QPushButton::clicked, this, [=] {CanThread::getInstance().sendSweeperLevels(3);});
}

/**
 * @brief Destructor of MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief The timerEvent of MainWindow
 * @param event : the active timer event
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    ResultModel* result = static_cast<ResultModel*>(ui->resultView->model());
    if(event->timerId() == _timerId)
    {
        if(_workMode == WORK_MODE::SHOW)
            FileCamera::getInstance().acquireImage(true, _analysis? ui->analysisWidget : nullptr, result);
        else
            GenCamera::getInstance().acquireImage(result);
    }
}

/**
 * @brief Enable the latest parameter, e.g. camera parameters, alg parameters etc.
 */
void MainWindow::updateParameter() {
    struct ParameterModel::Parameter& param = ParamManage::getInstance().model()->paramStruct();
    GenCamera& cam = GenCamera::getInstance();
    FileCamera::getInstance().setDir();
}


char BCDtoUINT (char p) {
    return ((p>>4)*10 + (p&0x0f));
}

void MainWindow::on_receive(QByteArray tmpdata) {
    qDebug() << "uart get:" << tmpdata.data();

    if (tmpdata[0] == 0xea) {

        if (tmpdata[1] == 0x02) {

            if (tmpdata[2] == 0x00) {

                if (tmpdata[3] == 0x90) {
                    ResultModel* result = static_cast<ResultModel*>(ui->resultView->model());
                    qDebug() << "enter genCamera" << QDateTime::currentDateTime();
                    GenCamera::getInstance().acquireImage(result);
                } else if (tmpdata[3] == 0x92) {
                    qDebug() << "uart send heart";
                    _serialPort->ack_status();
                }

            }


        } else if (tmpdata[1] == 0x0a) {


            QString date = "date -s \"" +
                            QString::number(BCDtoUINT(tmpdata.data()[4])*100 + BCDtoUINT(tmpdata.data()[5])) + "-" +
                            QString::number(BCDtoUINT(tmpdata.data()[6])) + "-" +
                            QString::number(BCDtoUINT(tmpdata.data()[7])) + " " +
                            QString::number(BCDtoUINT(tmpdata.data()[9])) + ":" +
                            QString::number(BCDtoUINT(tmpdata.data()[10])) + ":" +
                            QString::number(BCDtoUINT(tmpdata.data()[11])) + "\"";
            qDebug() << date;

            QProcess::startDetached(date);
            QProcess::startDetached("hwclock -w");
            QProcess::startDetached("sync");

            if (tmpdata[3] == 0x40) {
                _serialPort->ack_date();
            }


        } else if (tmpdata[1] == 0x03) {
            if (tmpdata[3] == 0x20) {
                if (tmpdata[4] == 0x01) {
                    //work mode
                    ui->workModeButton->setEnabled(false);
                    ui->showModeButton->setEnabled(true);
                    ui->debugModeButton->setEnabled(true);
                    _workMode = WORK_MODE::WORK;
                } else if (tmpdata[4] == 0x02) {
                    //show mode
                    ui->showModeButton->setEnabled(false);
                    ui->workModeButton->setEnabled(true);
                    ui->debugModeButton->setEnabled(true);
                    _workMode = WORK_MODE::SHOW;
                } else if (tmpdata[4] == 0x03) {
                    //debug mode
                    ui->debugModeButton->setEnabled(false);
                    ui->workModeButton->setEnabled(true);
                    ui->showModeButton->setEnabled(true);
                    _workMode = WORK_MODE::DEBUG;
                }
                _serialPort->ack_mode();
            } else if (tmpdata[3] == 0x10) {
                if (tmpdata[4] == 0x01) {
                    ui->playButton->setText("停止");

                    ResultModel* result = static_cast<ResultModel*>(ui->resultView->model());
                    if (_workMode == WORK_MODE::WORK) {
//                        GLOBAL_YOLO = 1;
                    } else if (_workMode == WORK_MODE::SHOW) {
                        FileCamera::getInstance().acquireImage(true, _analysis? ui->analysisWidget : nullptr, result);
                    } else if (_workMode == WORK_MODE::DEBUG){
                        GLOBAL_YOLO = 0;
                    }

                } else if (tmpdata[4] == 0x02) {
                    ui->playButton->setText("启动");
                }
            } else if (tmpdata[3] == 0x80) {
                GLOBAL_SPEED = tmpdata[4];
                ParamManage::getInstance().model()->paramStruct().aec.speed = tmpdata[4];
                ParamManage::getInstance().model()->getRootItem()->child(0)->child(4)->setData(root["aec"]["speed"].asInt(), 1);

                qDebug() << GLOBAL_SPEED;
                _serialPort->ack_speed();
            } else if (tmpdata[3] == 0x70) {
                if (tmpdata[4] & 0x01) {
                    GLOBAL_SAVEPICTURE = 1;
                    ParamManage::getInstance().model()->getRootItem()->child(2)->child(3)->setData(true, 1);

                } else {
                    GLOBAL_SAVEPICTURE = 0;
                    ParamManage::getInstance().model()->getRootItem()->child(2)->child(3)->setData(false, 1);
                }

                if (tmpdata[4] & 0x02) {
                    GLOBAL_YOLO = 1;
                } else {
                    GLOBAL_YOLO = 0;
                }


                _serialPort->ack_save();
            }

        } else if (tmpdata[1] == 0x0c) {
            ParamManage::getInstance().model()->paramStruct().aec.expTime_b = (tmpdata[4]*256 + tmpdata[5])*1000;

        }
            qDebug() << "recev param";
            

    } else if (tmpdata[0] == 0x61) {

        qDebug() << "last" << QDateTime::currentDateTime();
    } else if (tmpdata[0] == 0x53) {

        if (tmpdata[1] == 0xdd) {
            if (tmpdata[2] == 0x07) {

            }
        }
    }
}

// uchar checkData (char * a) {
//     int i = 
// }
