#ifndef STATUSMODEL_H
#define STATUSMODEL_H

#include <QAbstractTableModel>
#include <QVector>
class StatusModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit StatusModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QVector<QVariant>> _status;

public slots:
    void setSpeed(int speed);
    void setCamera(QString status);
    void setLight(QString status);
};

#endif // STATUSMODEL_H
