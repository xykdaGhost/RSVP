#include "PathEditor.h"
#include <QFileDialog>
/**
 * @brief Constructor of PathEditor
 * @param parent : the parent widget
 */
PathEditor::PathEditor(QWidget *parent) :
        QWidget(parent),
        pathEdit(new QLineEdit(this)),
        pathButton(new QPushButton("查看", this)),
        layout(new QHBoxLayout(this))
{
    layout->addWidget(pathEdit);
    layout->addWidget(pathButton);
    this->setLayout(layout);

    //open the folder selecting window
    connect(pathButton, &QPushButton::clicked, this, [=] {
        QString path;
        path = QFileDialog::getExistingDirectory(this, "选择目录", getData().toString());
        pathEdit->setText(path);
    });
}
/**
 * @brief Set data in the editing item
 * @param data : the data to set
 */
void PathEditor::setData(QVariant data) {
    pathEdit->setText(data.toString());
}
/**
 * @brief Get data from the editing item
 * @return the data in the item
 */
QVariant PathEditor::getData() const {
    return pathEdit->text();
}
