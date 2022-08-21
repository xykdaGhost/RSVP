#include "TreeItem.h"

/**
 * @brief Constructor of TreeItem
 * @param data : the origin data
 * @param parent : the parent of item
 */
TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
    : m_itemData(data), m_parentItem(parent) {

}

/**
 * @brief Destructor of TreeItem
 */
TreeItem::~TreeItem() {
    qDeleteAll(m_childItems);
}

/**
 * @brief Append a child
 * @param item : the child item
 */
void TreeItem::appendChild(TreeItem *item) {
    m_childItems.append(item);
}

/**
 * @brief Get the child with row index
 * @param row : the row index of child
 * @return the child
 */
TreeItem *TreeItem::child(int row) {
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

/**
 * @brief Get the child count number
 * @return the child count
 */
int TreeItem::childCount() const {
    return m_childItems.count();
}

/**
 * @brief Get the row number of this item in its parent item
 * @return the row number
 */
int TreeItem::row() const {
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}

/**
 * @brief Get the column count
 * @return the row count
 */
int TreeItem::columnCount() const {
    return m_itemData.count();
}

/**
 * @brief Get the data of this item
 * @param column : the column of data
 * @return the data
 */
QVariant TreeItem::data(int column) const {
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

/**
 * @brief Set the data of this item
 * @param data : data to set
 * @param column : the column of data
 */
void TreeItem::setData(QVariant data, int column) {
    if (column < 0 || column >= m_itemData.size())
        return;
    m_itemData[column] = data;
}

TreeItem *TreeItem::parentItem() {
    return m_parentItem;
}
