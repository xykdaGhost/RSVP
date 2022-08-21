#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    myUart(&Uart::getInstance()),
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

    //applyButton
    connect(ui->applyButton, &QPushButton::clicked, this, [=] {
        //on apply action, update the Json root and parameter struct and enable the new parameter
        ParamManage::getInstance().updateJsonRoot();
        ParamManage::getInstance().writeJsonToFile("settings.json");
        updateParameter();
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
