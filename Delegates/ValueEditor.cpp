#include "ValueEditor.h"
/**
 * @brief Constructor of ValueEditor
 * @param parent : the parent widget
 */
ValueEditor::ValueEditor(QWidget *parent) :
        QWidget(parent),
        addButton(new QPushButton("+", this)),
        valueEdit(new QLineEdit(this)),
        minusButton(new QPushButton("-", this)),
        layout(new QHBoxLayout(this)) {
    valueEdit->setAlignment(Qt::AlignCenter);
    layout->addWidget(minusButton);
    layout->addWidget(valueEdit);
    layout->addWidget(addButton);
    this->setLayout(layout);

    //add or minus value
    connect(minusButton, &QPushButton::clicked, this, [=] {
        int newValue = getData().toInt() - 1;
        if(newValue >= 0)
            setData(newValue);
    });

    connect(addButton, &QPushButton::clicked, this, [=] {
        setData(getData().toInt() + 1);
    });
}
/**
 * @brief Set data in the editing item
 * @param data : the data to set
 */
void ValueEditor::setData(QVariant data) {
    QString str = QString::number(data.toInt());
    valueEdit->setText(str);
}
/**
 * @brief Get data from the editing item
 * @return the data in the item
 */
QVariant ValueEditor::getData() const {
    return valueEdit->text().toInt();
}
