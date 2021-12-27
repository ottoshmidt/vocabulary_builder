#include "modelidioms.h"
#include "database.h"

#include <QDateTime>
#include <QSqlRecord>


ModelIdioms::ModelIdioms(QObject *parent, QSqlDatabase db) :
  QSqlTableModel(parent, db)
{
  setTable("idioms");

  setEditStrategy(QSqlTableModel::OnFieldChange);

  setHeaderData(0, Qt::Horizontal, tr("ID"));
  setHeaderData(1, Qt::Horizontal, tr("Idiom"));
  setHeaderData(2, Qt::Horizontal, tr("Definition"));
  setHeaderData(3, Qt::Horizontal, tr("Rating"));
  setHeaderData(4, Qt::Horizontal, tr("Mastered"));
  setHeaderData(6, Qt::Horizontal, tr("Update Time"));
  setHeaderData(7, Qt::Horizontal, tr("Create Time"));

  setFilter("language_fk = " + QString::number(
                          DataBase::getSetting("current_language")));
  select();

  connect(this, &QSqlTableModel::beforeUpdate, this, &ModelIdioms::onBeforeUpdate);
}

QVariant ModelIdioms::data(const QModelIndex &index, int role) const
{
  if ((index.column() == 0 || index.column() == 3) && role == Qt::TextAlignmentRole)
    return Qt::AlignCenter;
  else
    return QSqlTableModel::data(index, role);
}

void ModelIdioms::onBeforeUpdate(int row, QSqlRecord &record)
{
  Q_UNUSED(row)

  QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

  record.setValue("updatetime", dateTime);
  record.setGenerated("updatetime", true);
}
