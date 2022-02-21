#include "StatusModel.h"
#include <QSize>
StatusModel::StatusModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    _status.push_back({"相机状态：", "未连接"});
    _status.push_back({"光源状态：", "未连接"});
    _status.push_back({"实时车速(km/h)：", 0});
}

QVariant StatusModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //return header data by role
    switch(role) {
    case Qt::TextAlignmentRole:
        //align center for font
        return Qt::AlignCenter;
        break;
    case Qt::DisplayRole:
        //set horizontal header data
        if(orientation == Qt::Orientation::Vertical) {
            return _status[section][0];
        }
    case Qt::EditRole:
        if(orientation == Qt::Orientation::Vertical) {
            return _status[section][0];
        }
    default:
        return QVariant();
    }
}

int StatusModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _status.size();
}

int StatusModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return _status[0].size() - 1;
}

QVariant StatusModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(index.column() > 0 && index.row() > 1)
        return QVariant();

    switch(role) {
    case Qt::TextAlignmentRole:
        return Qt::AlignCenter;
    case Qt::DisplayRole:
        return _status[index.row()][index.column() + 1];
    case Qt::EditRole:
        return _status[index.row()][index.column() + 1];
    default:
        return QVariant();
    }
}

void StatusModel::setSpeed(int speed) {
    _status[2][1] = speed;
    emit dataChanged(createIndex(0,0), createIndex(rowCount(), columnCount()));
}
void StatusModel::setCamera(QString status) {
    _status[0][1] = status;
    emit dataChanged(createIndex(0,0), createIndex(rowCount(), columnCount()));
}
void StatusModel::setLight(QString status) {
    _status[1][1] = status;
    emit dataChanged(createIndex(0,0), createIndex(rowCount(), columnCount()));
}
