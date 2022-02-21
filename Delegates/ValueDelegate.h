#ifndef VALUEDELEGATE_H
#define VALUEDELEGATE_H

#include <QStyledItemDelegate>
/**
 * @brief The ValueDelegate of parameter view
 */
class ValueDelegate : public QStyledItemDelegate
{
public:
    ValueDelegate(QObject* parent = nullptr);

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget* editor, const QModelIndex& index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                        const QModelIndex& index) const Q_DECL_OVERRIDE;
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const Q_DECL_OVERRIDE;

};

#endif // VALUEDELEGATE_H
