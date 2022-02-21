#ifndef CHOOSEEDITOR_H
#define CHOOSEEDITOR_H

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLayout>
#include <QVariant>
/**
 * @brief The ChooseEditor class for editing bool type value
 */
class ChooseEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ChooseEditor(QWidget *parent = nullptr);
    void setData(QVariant data);
    QVariant getData() const;

private:
    QButtonGroup* buttonGroup;
    QRadioButton* trueButton;
    QRadioButton* falseButton;

    QHBoxLayout* layout;
signals:

};

#endif // CHOOSEEDITOR_H
