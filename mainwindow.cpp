#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    myUart(&Uart::getInstance()),
    _analysis(false),
    param(ParamManage::getInstance())
{
    ui->setupUi(this);

    //set param
    ValueDelegate* delegate = new ValueDelegate(this);
    ui->parameterView->setModel(param.model());
    ui->parameterView->expandAll();
    ui->parameterView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->parameterView->setItemDelegate(delegate);
    ui->parameterView->setEditTriggers(QAbstractItemView::AllEditTriggers);

    ResultModel* result = new ResultModel(this);
    ui->resultView->setFont(QFont("", 16));
    ui->resultView->setModel(result);
    ui->resultView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->resultView->setAlternatingRowColors(true);

    //interact part

    //applyButton
    connect(ui->applyButton, &QPushButton::clicked, this, [=] {
        //on apply action, update the Json root and parameter struct and enable the new parameter
        ParamManage::getInstance().updateJsonRoot();
        ParamManage::getInstance().writeJsonToFile("settings.json");
        updateParameter();
    });

    //display image
    connect(&FileCamera::getInstance(), &FileCamera::sendImage, [=] (QImage image) {
        if(ui->pictureLabel->width() / ui->pictureLabel->height() > image.width() / image.height())
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(image.width() * ui->pictureLabel->height() / image.height(), ui->pictureLabel->height())));
        else
            ui->pictureLabel->setPixmap(QPixmap::fromImage(image.scaled(ui->pictureLabel->width(), image.height() * ui->pictureLabel->width() / image.width())));
    });

    connect(ui->nextButton, &QPushButton::clicked, this, [=] {
        FileCamera::getInstance().acquireImage(true, result);
    });

}


/**
 * @brief Enable the latest parameter, e.g. camera parameters, alg parameters etc.
 */
void MainWindow::updateParameter() {
    struct ParameterModel::Parameter& param = ParamManage::getInstance().model()->paramStruct();
//    GenCamera& cam = GenCamera::getInstance();
//    FileCamera::getInstance().setDir();
}


MainWindow::~MainWindow()
{
    delete ui;
}

