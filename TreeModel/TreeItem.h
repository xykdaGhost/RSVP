#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>
/**
 * @brief The item in the tree model
 */
class TreeItem {
public:
    explicit TreeItem(const QVector<QVariant> &data, TreeItem *parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem *child);

    TreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setData(QVariant data, int column);
    int row() const;
    TreeItem *parentItem();

private:
    QVector<TreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TreeItem *m_parentItem;
};

#endif // TREEITEM_H
