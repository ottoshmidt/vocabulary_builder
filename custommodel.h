#ifndef CUSTOMMODEL_H
#define CUSTOMMODEL_H

#include <QSqlTableModel>

class CustomModel : public QSqlTableModel
{
public:
  CustomModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase()) :
    QSqlTableModel(parent, db) {}

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

  QString showOrderByClause() const;
};

#endif // CUSTOMMODEL_H
