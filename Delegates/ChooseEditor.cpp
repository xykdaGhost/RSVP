#include "ChooseEditor.h"
/**
 * @brief Constructor of ChooseEditor
 * @param parent : the parent widget
 */
ChooseEditor::ChooseEditor(QWidget *parent) :
        QWidget(parent),
        buttonGroup(new QButtonGroup(this)),
        trueButton(new QRadioButton("true", this)),
        falseButton(new QRadioButton("false", this)),
        layout(new QHBoxLayout(this)) {
    //add widgets to the layout
    buttonGroup->addButton(trueButton);
    buttonGroup->addButton(falseButton);
    layout->addWidget(trueButton, Qt::AlignCenter);
    layout->addWidget(falseButton, Qt::AlignCenter);

    //set background to hide the orgin value
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, Qt::black);
    this->setAutoFillBackground(true);
    this->setPalette(pal);

}

/**
 * @brief Set data in the editing item
 * @param data : the data to set
 */
void ChooseEditor::setData(QVariant data) {
    if(data.toBool())
        trueButton->setChecked(true);
    else
        falseButton->setChecked(true);
}

/**
 * @brief Get data from the editing item
 * @return the data in the item
 */
QVariant ChooseEditor::getData() const {
    return trueButton->isChecked();
}
