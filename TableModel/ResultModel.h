#ifndef RESULTMODEL_H
#define RESULTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <vector>
/**
 * @brief The result model in a table
 */
class ResultModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ResultModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;


    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void setData(std::vector<std::pair<int, double>> data);


private:
    QVector<QVector<QVariant>> _results;
    QVariant _maxArea;
    QVariant _minArea;


    int _trashLevel;
    int getTrashClass();
    int getTrash();

};

#endif // RESULTMODEL_H
