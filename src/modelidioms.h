#ifndef MODELIDIOMS_H
#define MODELIDIOMS_H

#include <QSqlTableModel>

class ModelIdioms : public QSqlTableModel
{
public:
  explicit ModelIdioms(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());

private:
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  void onBeforeUpdate(int row, QSqlRecord &record);
};

#endif // MODELIDIOMS_H
