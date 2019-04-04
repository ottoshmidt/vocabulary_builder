#ifndef SETTINGS_H
#define SETTINGS_H

#include "customtableview.h"
#include "checkboxdelegate.h"
#include "modelview.h"

#include <QSqlTableModel>
#include <QDialog>
#include <QPushButton>
#include <QCheckBox>
#include <QGridLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLabel>

class DialogSettings : public QDialog
{
  Q_OBJECT

public:
  explicit DialogSettings(QWidget *parent = nullptr);
  ~DialogSettings();

private:
  QGridLayout glSettings;
  QPushButton pbSettingsClose;
  QListWidget listWidgetSettings;
  QListWidget treeWdgUrls;
  QLabel *labelLangStngs;
  QListWidgetItem *listItemGeneral;
  QListWidgetItem *listItemWebsites;
  QListView listViewLanguages;
  QWidget widgetSettingsLanguages;

  QWidget widgetSettingsGeneral;
  QVBoxLayout glSettingsGeneral;
  QCheckBox cbAlternatigRows;
  QCheckBox cbConfirmDelete;
  QCheckBox cbStatusBar;

  CustomTableView *tableWebsites;
  QSqlTableModel *modelWebsites;
  QGridLayout glSettingsLanguage;
  QPushButton pbWebsiteAdd;
  QPushButton pbWebsiteDelete;

  CheckBoxDelegate *cbDelegate;
  CustomTableView *tableWords;

  void setupWidgets();
  void setupLayout();
  void setupModel();
  void setupView();

  bool firstRun = true;

private slots:
  void onWebsiteAdd();
  void onWebsiteDelete();
  void onSettingsRowChange(int row);
  void switchAlternatingRowColors(bool checked);
  void switchConfirmDelete(bool checked);
  void switchStatusBar(bool checked);
  void onBeforeInsert(QSqlRecord &record);

signals:
  void toggleConfirmDelete(bool ok);
  void toggleStatusBar(bool ok);
};

#endif // SETTINGS_H
