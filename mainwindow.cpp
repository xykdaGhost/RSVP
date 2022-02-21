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

int GLOBAL_SPEED = 0;
char canMessage = 23;


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
        qDebug() << "availableSize:" << text;
        _serialPort->ask_shoot();
        ui->sizeLabel->setText(text);
    });


    connect(&SerialPort::getInstance(), SIGNAL(receive_data(QByteArray)), this, SLOT(on_receive(QByteArray)), Qt::QueuedConnection);


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


void MainWindow::on_receive(QByteArray tmpdata) {
    qDebug() << "uart get:" << tmpdata.data();
    if (tmpdata[0] == 0x60) {
        if (tmpdata[1] == 0xec) {
           ResultModel* result = static_cast<ResultModel*>(ui->resultView->model());
           qDebug() << "enter genCamera" << QDateTime::currentDateTime();
           GenCamera::getInstance().acquireImage(result);
//            _timerId = startTimer(500);
        }
    } else if (tmpdata[0] == 0x61) {
        qDebug() << "last" << QDateTime::currentDateTime();
    } else if (tmpdata[0] == 0x53) {
        if (tmpdata[1] == 0xdd) {
            if (tmpdata[2] == 0x07) {

            }
        }
    }
}

