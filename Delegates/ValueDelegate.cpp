#include "ValueDelegate.h"
#include "ValueEditor.h"
#include "ChooseEditor.h"
#include "PathEditor.h"
/**
 * @brief Constructor of ValueDelegate
 * @param parent : parent object
 */
ValueDelegate::ValueDelegate(QObject* parent) :
    QStyledItemDelegate(parent) {

}

/**
 * @brief Create an editor for the view, value in different types should have different editor
 * @param parent : parent widget
 * @param option : not used
 * @param index : the index of editor
 * @return the editor widget created
 */
QWidget* ValueDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const {
    if(index.column() == 0)
        return nullptr;
    QWidget* editor;
    //create different editor for different value type
    switch (index.data().type()) {
    case QVariant::Type::Int:
        editor = new ValueEditor(parent);
        break;
    case QVariant::Type::Bool:
        editor = new ChooseEditor(parent);
        break;
    case QVariant::Type::String:
        if(index.data() != "")
            editor = new PathEditor(parent);
        else
            editor = new QWidget(parent);
        break;
    default:
        editor = nullptr;
        break;
    }
    return editor;
}

/**
 * @brief Set the data from model to editor.
 * @param editor : the editor widget pointer
 * @param index : the index of model
 */
void ValueDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    //convert editor to particular type and set editor data from model
    switch (index.data().type()) {
    case QVariant::Type::Bool:
        static_cast<ChooseEditor*>(editor)->setData(
                    index.model()->data(index, Qt::EditRole).toBool());
        break;
    case QVariant::Type::Int:
        static_cast<ValueEditor*>(editor)->setData(
                    index.model()->data(index, Qt::EditRole).toInt());
        break;
    case QVariant::Type::String:
        if(index.data() != "")
            static_cast<PathEditor*>(editor)->setData(
                    index.model()->data(index, Qt::EditRole).toString());
        break;
    default:

        break;
    }

}

/**
 * @brief Set the data from editor to model
 * @param editor : the editor widget pointer
 * @param model : the model to edit
 * @param index : the index of model
 */
void ValueDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                 const QModelIndex &index) const {
    int test = 0;
    switch (index.data().type()) {
    case QVariant::Type::Bool:
        model->setData(
                    index, static_cast<ChooseEditor*>(editor)->getData(), Qt::EditRole);
        break;
    case QVariant::Type::Int:
        test = static_cast<ValueEditor*>(editor)->getData().toInt();
        model->setData(
                    index, static_cast<ValueEditor*>(editor)->getData(), Qt::EditRole);
        break;
    case QVariant::Type::String:
        if(index.data() != "")
            model->setData(
                    index, static_cast<PathEditor*>(editor)->getData(), Qt::EditRole);
        break;
    default:

        break;
    }
}

/**
 * @brief Update the editor geometry to fit the view
 * @param editor : the editor widget pointer
 * @param option : the selected view
 * @param index : the model to edit
 */
void ValueDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}
