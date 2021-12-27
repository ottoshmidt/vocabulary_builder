#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "dialogaddword.h"
#include "dialogfilter.h"
#include "dialogsettings.h"
#include "dialoggotoword.h"
#include "dialogconfirmdelete.h"
#include "modelidioms.h"
#include "tableidioms.h"

#include <QMainWindow>
#include <QWebEngineView>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QtSql>
#include <QTableView>
#include <QListWidget>
#include <QDialog>
#include <QCheckBox>
#include <QThread>
#include <QStatusBar>
#include <QDockWidget>
#include <QToolBar>


class WordCount : public QThread
{
public:
  WordCount(QLabel *statusBar) { bar = statusBar; }
  ~WordCount() {}
private:
  void run();

  QLabel *bar;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  QStringList urls;
  QVector<QWebEngineView*> webViewList;

  QMenu menuMain;
  QMenu menuWords;
  QMenu menuMasteredFilter;
  QMenu menuDefinedFilter;

  QAction quitAction;
  QAction actionClearBrowsers;
  QAction actionSettings;
  QAction actionAddWord;
  QAction actionRefresh;
  QAction actionfilterWord;
  QAction actionMastered;
  QAction actionUnmastered;
  QAction actionDefined;
  QAction actionUndefined;
  QAction actionGotoWord;
  QAction actionDeleteRecord;
  QAction actionNextTab;
  QAction actionPrevTab;

  QToolBar toolBar;

  QActionGroup *actionsLanguages;
  QList<QAction*> languages;

  QScrollArea centralScroll;
  QWidget centreWidget;
  QGridLayout glCentralGrid;
  QTabWidget tabWidget;

  ModelView modelView;
  CustomModel *modelWords;
  CustomTableView *tableWords;
  ModelIdioms modelIdioms;
  TableIdioms tableIdioms;
  QSqlTableModel *currentModel;
  QTableView *currentView;

  DialogAddWord *dialogAddWord;
  DialogFilter *dialogfilterWord;
  DialogSettings *dialogSettings;
  DialogGotoWord *dialogGotoWord;
  DialogConfirmDelete *dialogConfirmDelete;

  QStatusBar *statBar;
  QLabel lbStatBar;
  WordCount *threadWordCount;

  void setupModelView();
  void setupCentralGrid();
  void setupDialogs();
  void setupMenus();
  void updateUrls();
  void addWebViews();

  int row = 1;
  int col = 0;
  int currentWebViews = 0;

  void webSearchWord(const QString &word);
  void clearBrowsers();

  void resizeEvent(QResizeEvent *event) override;
  void showEvent(QShowEvent *event) override;

private slots:
  void resizeRowsColumns();
  void onSelectionChanged(const QItemSelection &selected,
                          const QItemSelection &deselected);
  void propagateTabIndex(int index);
  void onMasteredToggle(const bool &b);
  void onUnmasteredToggle(const bool &b);
  void onDefinedToggle(const bool &b);
  void onUndefinedToggle(const bool &b);
  void applyFilter();
  void confirmDeleteItem();
  void deleteRecord(int row);
  void updateWordCount();
  void onBeforeUpdate(int row, QSqlRecord &record);
  void toggleConfirmDelete(bool ok);
  void toggleStatusBar(bool ok);
  void updateModel();
  void actionLanguage(const QAction *action);
  void onChangeTabKey();
};

#endif // MAINWINDOW_H
