#ifndef PATHEDITOR_H
#define PATHEDITOR_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
/**
 * @brief The PathEditor class for editing file path(string) type value.
 */
class PathEditor : public QWidget
{
    Q_OBJECT
public:
    explicit PathEditor(QWidget *parent = nullptr);
    void setData(QVariant data);
    QVariant getData() const;

private:
    QLineEdit* pathEdit;
    QPushButton* pathButton;
    QHBoxLayout* layout;
signals:

};

#endif // PATHEDITOR_H
