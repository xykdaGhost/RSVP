#include "TreeModel.h"
#include <QSize>
/**
 * @brief Constructor of TreeModel
 * @param root : json root for initial setup
 * @param parent : the parent object
 */
TreeModel::TreeModel(const Json::Value root, QObject *parent)
    : QAbstractItemModel(parent) {

}

/**
 * @brief Destructor of TreeModel
 */
TreeModel::~TreeModel()
{
    delete rootItem;
}

/**
 * @brief Get the index by the row and column infomation of parent index
 * @param row : the row of index
 * @param column : the column of index
 * @param parent : the parent index
 * @return the index
 */
QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    //check for the parent index validation
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    //find the child of parent and create the index
    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

/**
 * @brief Get the parent index of an index
 * @param index : the child index
 * @return the parent index
 */
QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    //find the parent of child index
    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

/**
 * @brief Get the row count of the index
 * @param parent : the parent index
 * @return the row count of index
 */
int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

/**
 * @brief Get the column count of the index
 * @param parent : the parent index
 * @return the column count of index
 */
int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

/**
 * @brief Get the data of the index
 * @param index : the index of data
 * @param role : the data role
 * @return : the data
 */
QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role) {
    //set font text align center for the data and align left for the header
    case Qt::TextAlignmentRole:
        if(index.column() != 0)
            return Qt::AlignCenter;
        else
            return int(Qt::AlignLeft | Qt::AlignVCenter);
        break;
    //set table size
    case Qt::SizeHintRole:
        return QSize(250, 60);
    //get data by column
    case Qt::DisplayRole:
        return static_cast<TreeItem*>(index.internalPointer())->data(index.column());
    case Qt::EditRole:
        return static_cast<TreeItem*>(index.internalPointer())->data(index.column());
    default:
        return QVariant();
    }
}

/**
 * @brief Set data by index
 * @param index : the data index
 * @param value : the data value
 * @param role : the data role
 * @return success or fail
 */
bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    static_cast<TreeItem*>(index.internalPointer())->setData(value, index.column());
    return true;
}

/**
 * @brief Get the flag of index
 * @param index : the index
 * @return the flags
 */
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    //add editable flag to every index
    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

/**
 * @brief Get header data
 * @param section : the section of header
 * @param orientation : the orientation of header, Horizontal or Vertical
 * @param role : data role
 * @return the header data
 */
QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}


