#include "dialogsettings.h"
#include "database.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>


DialogSettings::DialogSettings(QWidget *parent)
{
  setParent(parent);
  setWindowFlags(Qt::Dialog);
  setWindowTitle(tr("Settings"));
  setMinimumSize(750, 400);

  setupWidgets();
  setupModel();
  setupView();
  setupLayout();
}

DialogSettings::~DialogSettings()
{
}

void DialogSettings::setupWidgets()
{
  treeWidgetSettings.setFixedSize(110, 370);
  treeWidgetSettings.setHeaderHidden(true);
  connect(&treeWidgetSettings, &QTreeWidget::currentItemChanged, this,
          &DialogSettings::onSettingsRowChange);

  treeItemGeneral = new QTreeWidgetItem(&treeWidgetSettings);
  treeItemGeneral->setText(0, tr("General"));
  treeItemWebsites = new QTreeWidgetItem(&treeWidgetSettings);
  treeItemWebsites->setText(0, tr("Websites"));

  QSqlQuery query(DataBase::getDb());

  if(!query.exec("select language from languages"))
    QMessageBox::critical(nullptr, tr("Database Error when getting languages"),
                          query.lastError().text());

  while(query.next())
    new QTreeWidgetItem(treeItemWebsites, QStringList(query.value(0).toString()));

  cbAlternatigRows.setText(tr("Alternating row colours"));
  connect(&cbAlternatigRows, &QCheckBox::toggled, this,
          &DialogSettings::switchAlternatingRowColors);

  cbConfirmDelete.setText(tr("Confirm when deleting word"));
  connect(&cbConfirmDelete, &QCheckBox::toggled, this,
          &DialogSettings::switchConfirmDelete);

  pbLanguageAdd.setText("Add");
  connect(&pbLanguageAdd, &QPushButton::clicked, this,
          &DialogSettings::onLanguageAdd);

  cbStatusBar.setText(tr("Show status bar with word statistics"));
  connect(&cbStatusBar, &QCheckBox::toggled, this,
          &DialogSettings::switchStatusBar);

  tableWords = ModelView::view();

  bool b = DataBase::getSetting("alternating_row_colors");
  cbAlternatigRows.setChecked(b);
  tableWords->setAlternatingRowColors(b);

  b = DataBase::getSetting("confirm_delete");
  cbConfirmDelete.setChecked(b);
  extern bool confirmDelete;
  confirmDelete = b;

  b = DataBase::getSetting("status_bar");
  cbStatusBar.setChecked(b);
  extern bool showStatusBar;
  showStatusBar = b;

  labelWebsites = new QLabel(&widgetSettingsWebsites);
  labelWebsites->setText(tr("When adding a website insert '{word}' (without quotes)"
                          " in the url, where a search word should be placed."));

  pbSettingsClose.setParent(this);
  pbSettingsClose.setText(tr("Close"));
  connect(&pbSettingsClose, &QPushButton::released, this, &QDialog::close);

  pbWebsiteAdd.setParent(&widgetSettingsWebsites);
  pbWebsiteAdd.setText(tr("Add"));
  connect(&pbWebsiteAdd, &QPushButton::released, this,
          &DialogSettings::onWebsiteAdd);
  pbWebsiteDelete.setParent(&widgetSettingsWebsites);
  pbWebsiteDelete.setText(tr("Delete"));
  connect(&pbWebsiteDelete, &QPushButton::released, this,
          &DialogSettings::onWebsiteDelete);

  widgetSettingsWebsites.hide();
}

void DialogSettings::setupLayout()
{
  glSettingsGeneral.addWidget(&cbAlternatigRows, 0, 0, 1, 3);
  glSettingsGeneral.addWidget(&cbConfirmDelete, 1, 0, 1, 3);
  glSettingsGeneral.addWidget(&cbStatusBar, 2, 0, 1, 3);
  glSettingsGeneral.addWidget(&pbLanguageAdd, 3, 0);
  glSettingsGeneral.addWidget(tableLanguages, 4, 0, 1, 2);
  glSettingsGeneral.setColumnStretch(1, 1);
  glSettingsGeneral.setColumnStretch(2, 9);
  glSettingsGeneral.setRowStretch(5, 1);
  widgetSettingsGeneral.setLayout(&glSettingsGeneral);

  glSettingsLanguage.addWidget(labelWebsites, 0, 0, 1, 3);
  glSettingsLanguage.addWidget(&pbWebsiteAdd, 1, 0);
  glSettingsLanguage.addWidget(&pbWebsiteDelete, 1, 1);
  glSettingsLanguage.addWidget(tableWebsites, 2, 0, 1, 3);
  glSettingsLanguage.setColumnStretch(2, 1);
  widgetSettingsWebsites.setLayout(&glSettingsLanguage);

  glSettings.addWidget(&treeWidgetSettings, 0, 0);
  glSettings.addWidget(&widgetSettingsGeneral, 0, 1, 1, 2);
  glSettings.addWidget(&widgetSettingsWebsites, 0, 1, 1, 2);
  glSettings.addWidget(&pbSettingsClose, 1, 2);
  glSettings.setColumnStretch(0, 0);
  glSettings.setColumnStretch(1, 1);

  setLayout(&glSettings);

  treeWidgetSettings.setCurrentItem(treeItemGeneral);
}

void DialogSettings::setupModel()
{
  modelLanguages = new QSqlTableModel(&widgetSettingsGeneral, DataBase::getDb());
  modelLanguages->setTable("languages");
  modelLanguages->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelLanguages->setHeaderData(1, Qt::Horizontal, tr("Language"));
  modelLanguages->select();

  modelWebsites = new QSqlRelationalTableModel(&widgetSettingsWebsites,
                                               DataBase::getDb());
  modelWebsites->setTable("urls");
  modelWebsites->setRelation(3, QSqlRelation("languages", "id", "language"));
  modelWebsites->setEditStrategy(QSqlTableModel::OnFieldChange);
  modelWebsites->setHeaderData(0, Qt::Horizontal, tr("ID"));
  modelWebsites->setHeaderData(1, Qt::Horizontal, tr("URL"));
  modelWebsites->setHeaderData(2, Qt::Horizontal, tr("Enabled"));
  modelWebsites->select();
}

void DialogSettings::setupView()
{
  tableLanguages = new QTableView(&widgetSettingsGeneral);
  tableLanguages->setModel(modelLanguages);
  tableLanguages->hideColumn(0);
  tableLanguages->resizeRowsToContents();

  tableWebsites = new CustomTableView(&widgetSettingsWebsites);
  cbDelegate = new CheckBoxDelegate(tableWebsites);

  tableWebsites->setItemDelegateForColumn(2, cbDelegate);
  tableWebsites->setSelectionMode(QAbstractItemView::SingleSelection);
  tableWebsites->setModel(modelWebsites);
  tableWebsites->hideColumn(0);
  tableWebsites->hideColumn(3);
  tableWebsites->horizontalHeader()->moveSection(1, 2);
  tableWebsites->verticalHeader()->hide();
  tableWebsites->resizeColumnToContents(2);
  tableWebsites->setColumnWidth(1, width() / 3 * 2);
  tableWebsites->resizeRowsToContents();
}

void DialogSettings::showEvent(QShowEvent *event)
{
  extern bool confirmDelete;

  cbConfirmDelete.setChecked(confirmDelete);

  event->accept();
}

void DialogSettings::onSettingsRowChange(QTreeWidgetItem *current,
                                         QTreeWidgetItem *previous)
{
  if (current->text(0) == "General")
  {
    widgetSettingsGeneral.show();
    widgetSettingsWebsites.hide();
  }
  else if (current->text(0) == "Websites")
  {
    widgetSettingsGeneral.hide();
    widgetSettingsWebsites.hide();
  }
  else
  {
    widgetSettingsGeneral.hide();
    widgetSettingsWebsites.show();

    QString lang = current->text(0);

    QSqlQuery query(DataBase::getDb());

    query.prepare("select id from languages where language = ?");
    query.addBindValue(lang);

    if(!query.exec())
      QMessageBox::critical(nullptr, tr("Database Error in getting language id"),
                            query.lastError().text());

    while(query.next())
      languageID = query.value(0).toInt();

    modelWebsites->setFilter("relTblAl_3.language = '" + lang + "'");
    tableWebsites->resizeRowsToContents();
  }

  Q_UNUSED(previous)
}

void DialogSettings::switchAlternatingRowColors(bool checked)
{
  tableWords->setAlternatingRowColors(checked);

  if(!firstRun)
    DataBase::updateSetting("alternating_row_colors", checked);

  firstRun = false;
}

void DialogSettings::switchConfirmDelete(bool checked)
{
  if(!firstRun)
    DataBase::updateSetting("confirm_delete", checked);

  emit toggleConfirmDelete(checked);

  firstRun = false;
}

void DialogSettings::switchStatusBar(bool checked)
{
  if(!firstRun)
    DataBase::updateSetting("status_bar", checked);

  emit toggleStatusBar(checked);

  firstRun = false;
}

void DialogSettings::onLanguageAdd()
{
  modelLanguages->insertRow(modelLanguages->rowCount());
}

void DialogSettings::onWebsiteAdd()
{
  int row = modelWebsites->rowCount();

  modelWebsites->insertRow(row);
  modelWebsites->setData(modelWebsites->index(row, 2), 1);
  modelWebsites->setData(modelWebsites->index(row, 3), languageID);
}

void DialogSettings::onWebsiteDelete()
{
  int r = tableWebsites->currentIndex().row();
  modelWebsites->removeRow(r);
  modelWebsites->select();
}
