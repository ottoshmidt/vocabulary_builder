#include "custommodel.h"

QVariant CustomModel::data(const QModelIndex &index, int role) const
{
  if (index.column() == 5 && role == Qt::TextAlignmentRole)
    return Qt::AlignCenter;
  else
    return QSqlTableModel::data(index, role);
}

QString CustomModel::showOrderByClause() const
{
  return  orderByClause();
}
