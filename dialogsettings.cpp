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
  listWidgetSettings.setFixedSize(90, 370);
  connect(&listWidgetSettings, &QListWidget::currentRowChanged, this,
          &DialogSettings::onSettingsRowChange);

  listItemGeneral = new QListWidgetItem(&listWidgetSettings);
  listItemGeneral->setText(tr("General"));
  listItemWebsites = new QListWidgetItem(&listWidgetSettings);
  listItemWebsites->setText(tr("Websites"));

  cbAlternatigRows.setText(tr("Alternating row colours"));
  connect(&cbAlternatigRows, &QCheckBox::toggled, this,
          &DialogSettings::switchAlternatingRowColors);

  cbConfirmDelete.setText(tr("Confirm when deleting word"));
  connect(&cbConfirmDelete, &QCheckBox::toggled, this,
          &DialogSettings::switchConfirmDelete);

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

  labelLangStngs = new QLabel(&widgetSettingsLanguages);
  labelLangStngs->setText(tr("When adding a website insert '{word}' (without quotes)"
                          " in the url, where a search word should be placed."));

  pbSettingsClose.setParent(this);
  pbSettingsClose.setText(tr("Close"));
  connect(&pbSettingsClose, &QPushButton::released, this, &QDialog::close);

  pbWebsiteAdd.setParent(&widgetSettingsLanguages);
  pbWebsiteAdd.setText(tr("Add"));
  connect(&pbWebsiteAdd, &QPushButton::released, this, &DialogSettings::onWebsiteAdd);
  pbWebsiteDelete.setParent(&widgetSettingsLanguages);
  pbWebsiteDelete.setText(tr("Delete"));
  connect(&pbWebsiteDelete, &QPushButton::released, this,
          &DialogSettings::onWebsiteDelete);

  widgetSettingsLanguages.hide();
}

void DialogSettings::setupLayout()
{
  glSettingsGeneral.addWidget(&cbAlternatigRows, 0);
  glSettingsGeneral.addWidget(&cbConfirmDelete, 1);
  glSettingsGeneral.addWidget(&cbStatusBar, 2);
  glSettingsGeneral.addStretch(1);
  widgetSettingsGeneral.setLayout(&glSettingsGeneral);

  glSettingsLanguage.addWidget(labelLangStngs, 0, 0, 1, 3);
  glSettingsLanguage.addWidget(&pbWebsiteAdd, 1, 0);
  glSettingsLanguage.addWidget(&pbWebsiteDelete, 1, 1);
  glSettingsLanguage.addWidget(tableWebsites, 2, 0, 1, 3);
  glSettingsLanguage.setColumnStretch(2, 1);
  widgetSettingsLanguages.setLayout(&glSettingsLanguage);

  glSettings.addWidget(&listWidgetSettings, 0, 0);
  glSettings.addWidget(&widgetSettingsGeneral, 0, 1, 1, 2);
  glSettings.addWidget(&widgetSettingsLanguages, 0, 1, 1, 2);
  glSettings.addWidget(&pbSettingsClose, 1, 2);
  glSettings.setColumnStretch(0, 0);
  glSettings.setColumnStretch(1, 1);

  setLayout(&glSettings);

  // Don't move from here (window will flicker on startup)
  listWidgetSettings.setCurrentRow(0);
}

void DialogSettings::setupModel()
{
  modelWebsites = new QSqlTableModel(&widgetSettingsLanguages, DataBase::getDb());
  modelWebsites->setTable("urls");
  modelWebsites->setEditStrategy(QSqlTableModel::OnFieldChange);
  connect(modelWebsites, &QSqlTableModel::beforeInsert, this,
          &DialogSettings::onBeforeInsert);

  modelWebsites->setHeaderData(0, Qt::Horizontal, tr("ID"));
  modelWebsites->setHeaderData(1, Qt::Horizontal, tr("URL"));
  modelWebsites->setHeaderData(2, Qt::Horizontal, tr("Enabled"));
  modelWebsites->select();
}

void DialogSettings::setupView()
{
  tableWebsites = new CustomTableView(&widgetSettingsLanguages);
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

void DialogSettings::onSettingsRowChange(int row)
{
  switch(row)
  {
    case 0:
      widgetSettingsLanguages.hide();
      widgetSettingsGeneral.show();
      break;
    case 1:
      widgetSettingsGeneral.hide();
      widgetSettingsLanguages.show();
      break;
  }
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

void DialogSettings::onBeforeInsert(QSqlRecord &record)
{
  if(!record.value("url").toString().isEmpty())
  {
    record.setValue("language_fl", 1);
    record.setValue("enabled", 1);

    tableWebsites->resizeRowsToContents();
  }
}

void DialogSettings::onWebsiteAdd()
{
  modelWebsites->insertRow(modelWebsites->rowCount());
}

void DialogSettings::onWebsiteDelete()
{
  int r = tableWebsites->currentIndex().row();
  modelWebsites->removeRow(r);
  modelWebsites->select();
}
