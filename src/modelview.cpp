#include "modelview.h"
#include "database.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

CustomModel *ModelView::modelWords;
CustomTableView *ModelView::tableWords;

ModelView::ModelView(QObject *parent) : QObject (parent)
{
  setupModel();

  setupView();
}

ModelView::~ModelView()
{
}

CustomModel *ModelView::model()
{
  return modelWords;
}

CustomTableView *ModelView::view()
{
  return tableWords;
}

void ModelView::setupModel()
{
  modelWords = new CustomModel(this, DataBase::getDb());

  modelWords->setTable("words");
  modelWords->setEditStrategy(QSqlTableModel::OnFieldChange);

  modelWords->setHeaderData(0, Qt::Horizontal, tr("ID"));
  modelWords->setHeaderData(1, Qt::Horizontal, tr("Word"));
  modelWords->setHeaderData(2, Qt::Horizontal, tr("Note"));
  modelWords->setHeaderData(3, Qt::Horizontal, tr("Pronunciation"));
  modelWords->setHeaderData(4, Qt::Horizontal, tr("Definition"));
  modelWords->setHeaderData(5, Qt::Horizontal, tr("Rating"));
  modelWords->setHeaderData(6, Qt::Horizontal, tr("Mastered"));
  modelWords->setHeaderData(8, Qt::Horizontal, tr("Update Time"));
  modelWords->setHeaderData(9, Qt::Horizontal, tr("Create Time"));

  modelWords->setFilter("language_fk = " + QString::number(
                          DataBase::getSetting("current_language")));

  modelWords->select();
}

void ModelView::setupView()
{
  tableWords = new CustomTableView();

  tableWords->setModel(modelWords);
  tableWords->setSortingEnabled(true);
  tableWords->sortByColumn(8, Qt::DescendingOrder);
  tableWords->hideColumn(7);

  cbDelegate = new CheckBoxDelegate(tableWords);
  tableWords->setItemDelegateForColumn(6, cbDelegate);
  tableWords->setSelectionMode(QAbstractItemView::SingleSelection);
}

