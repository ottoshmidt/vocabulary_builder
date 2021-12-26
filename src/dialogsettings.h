#ifndef SETTINGS_H
#define SETTINGS_H

#include "customtableview.h"
#include "checkboxdelegate.h"
#include "modelview.h"

#include <QSqlRelationalTableModel>
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QShowEvent>

class DialogSettings : public QDialog
{
  Q_OBJECT

public:
  DialogSettings(QWidget *parent = nullptr);
  ~DialogSettings();

private:
  QGridLayout glSettings;
  QPushButton pbSettingsClose;
  QTreeWidget treeWidgetSettings;
  QLabel *labelWebsites;
  QTreeWidgetItem *treeItemGeneral;
  QTreeWidgetItem *treeItemWebsites;
  QWidget widgetSettingsWebsites;

  QWidget widgetSettingsGeneral;
  QGridLayout glSettingsGeneral;
  QCheckBox cbAlternatigRows;
  QCheckBox cbConfirmDelete;
  QCheckBox cbStatusBar;

  CustomTableView *tableWebsites;
  QTableView *tableLanguages;
  QSqlRelationalTableModel *modelWebsites;
  QSqlTableModel *modelLanguages;
  QGridLayout glSettingsLanguage;
  QPushButton pbLanguageAdd;
  QPushButton pbWebsiteAdd;
  QPushButton pbWebsiteDelete;

  CheckBoxDelegate *cbDelegate;
  CustomTableView *tableWords;

  void setupWidgets();
  void setupLayout();
  void setupModel();
  void setupView();

  bool firstRun = true;

  int languageID;

  void showEvent(QShowEvent *event) override;

private slots:
  void onLanguageAdd();
  void onWebsiteAdd();
  void onWebsiteDelete();
  void onSettingsRowChange(QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void switchAlternatingRowColors(bool checked);
  void switchConfirmDelete(bool checked);
  void switchStatusBar(bool checked);

signals:
  void toggleConfirmDelete(bool ok);
  void toggleStatusBar(bool ok);
};

#endif // SETTINGS_H
