#ifndef VALUEEDITOR_H
#define VALUEEDITOR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
/**
 * @brief The ValueEditor class for editing int type value.
 */
class ValueEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ValueEditor(QWidget *parent = nullptr);
    void setData(QVariant data);
    QVariant getData() const;
private:
    QPushButton* addButton;
    QLineEdit* valueEdit;
    QPushButton* minusButton;
    QHBoxLayout* layout;
signals:

};

#endif // VALUEEDITOR_H
