#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include "TreeItem.h"
#include "jsoncpp/json/json.h"
/**
 * @brief The tree model
 */
class TreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    explicit TreeModel(const Json::Value root, QObject *parent = nullptr);
    virtual ~TreeModel();

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    virtual void setupModelData(const Json::Value root) = 0;


protected:
    TreeItem *rootItem;
};

#endif // TREEMODEL_H
