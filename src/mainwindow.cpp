#include "mainwindow.h"
#include "database.h"

#include <QMessageBox>
#include <QCheckBox>
#include <QScrollBar>
#include <QStatusBar>

//TODO: Dark theme
//TODO: Reflect website modifications in settings in already loaded programme
//TODO: Ability to save database in another location (Share drive, Dropbox, etc.).
//      Would need to save settings in .config dir
//TODO: Db file export/import
//TODO: Delete word on right-click and del button
//TODO: check if we can set mastered checkbox in the middle automatically
//TODO: Interface translations geo,rus
//TODO: About window
//TODO: pdf, csv, excel exports
//TODO: System tray icon (configurable from settings)
//TODO: Clear button in Filter dialog
//TODO: Configurable keyboard shortcuts

bool showStatusBar;
bool confirmDelete;

QWebEngineView *senderWebView = nullptr;
QString searchWord = "";

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent)
{
  setWindowIcon(QIcon(":icons/books.ico"));

  setupDialogs();
  setupModelView();
  setupCentralGrid();
  setupMenus();
}

void MainWindow::setupDialogs()
{
  dialogAddWord = new DialogAddWord(this);
  dialogfilterWord = new DialogFilter(this);
  dialogSettings = new DialogSettings(this);
  dialogGotoWord = new DialogGotoWord(this);
  dialogConfirmDelete = new DialogConfirmDelete(this);

  connect(dialogAddWord, &DialogAddWord::wordInserted, this,
          &MainWindow::updateWordCount);

  connect(dialogAddWord, &DialogAddWord::linkClicked, dialogGotoWord,
          &DialogGotoWord::gotoWord);

  connect(dialogAddWord, &DialogAddWord::wordUpdated, this,
          &MainWindow::updateModel);

  connect(dialogSettings, &DialogSettings::toggleConfirmDelete, this,
          &MainWindow::toggleConfirmDelete);

  connect(dialogSettings, &DialogSettings::toggleStatusBar, this,
          &MainWindow::toggleStatusBar);
}

void MainWindow::setupModelView()
{
  // To run the ModelView constructor (don't remove)
  modelView.setParent(this);

  tableWords = ModelView::view();
  tableWords->setParent(this);

  modelWords = ModelView::model();

  connect(tableWords, &CustomTableView::viewPortChanged, this,
          &MainWindow::resizeRowsColumns);

  connect(tableWords, &CustomTableView::selectionChangedSig, this,
          &MainWindow::onSelectionChanged);

  connect(modelWords, &QSqlTableModel::beforeUpdate, this,
          &MainWindow::onBeforeUpdate);

  modelIdioms.setParent(this);
  tableIdioms.setParent(this);
  tableIdioms.setModel(&modelIdioms);
  tableIdioms.hideColumn(5);

  connect(&tableIdioms, &TableIdioms::selectionChangedSig, this,
          &MainWindow::onSelectionChanged);
}

void MainWindow::setupCentralGrid()
{
  centreWidget.setLayout(&glCentralGrid);

  centralScroll.setWidget(&centreWidget);
  centralScroll.setWidgetResizable(true);

  setCentralWidget(&centralScroll);

  tabWidget.addTab(tableWords, "Words");
  tabWidget.addTab(&tableIdioms, "Idioms");
  glCentralGrid.addWidget(&tabWidget, 0, 0, 1, 2);

  updateUrls();
  addWebViews();

  statBar = new QStatusBar();
  statBar->addWidget(&lbStatBar);
  threadWordCount = new WordCount(&lbStatBar);
  setStatusBar(statBar);
  threadWordCount->start();
  toggleStatusBar(showStatusBar);

  propagateTabIndex(tabWidget.currentIndex());
  connect(&tabWidget, &QTabWidget::currentChanged, this,
          &MainWindow::propagateTabIndex);

  actionNextTab.setShortcut(Qt::CTRL + Qt::Key_PageDown);
  connect(&actionNextTab, &QAction::triggered, this, &MainWindow::onChangeTabKey);
  actionPrevTab.setShortcut(Qt::CTRL + Qt::Key_PageUp);
  connect(&actionPrevTab, &QAction::triggered, this, &MainWindow::onChangeTabKey);
  QList<QKeySequence> langKeys;
  langKeys.append(Qt::CTRL + Qt::Key_Period);
  langKeys.append(Qt::CTRL + Qt::Key_Greater);
  actionNextLanguage.setShortcuts(langKeys);
  connect(&actionNextLanguage, &QAction::triggered, this,
          &MainWindow::onNextLanguage);
  langKeys.clear();
  langKeys.append(Qt::CTRL + Qt::Key_Comma);
  langKeys.append(Qt::CTRL + Qt::Key_Less);
  actionPrevLanguage.setShortcuts(langKeys);
  connect(&actionPrevLanguage, &QAction::triggered, this,
          &MainWindow::onPrevLanguage);
  addAction(&actionNextTab);
  addAction(&actionPrevTab);
  addAction(&actionNextLanguage);
  addAction(&actionPrevLanguage);
}

void MainWindow::setupMenus()
{
  menuMain.setTitle(tr("&Main"));

  actionClearBrowsers.setText(tr("Clear Browsers"));
  actionClearBrowsers.setShortcut(QKeySequence(Qt::Key_F10));
  actionClearBrowsers.setIcon(QIcon(":/icons/blank.ico"));
  connect(&actionClearBrowsers, &QAction::triggered, this,
          &MainWindow::clearBrowsers);
  menuMain.addAction(&actionClearBrowsers);

  QMenu *languageMenu = menuMain.addMenu(QIcon(":/icons/language.ico"), "Language");
  actionsLanguages = new QActionGroup(languageMenu);
  QSqlQuery query(DataBase::getDb());

  if(!query.exec("select id, language from languages"))
    QMessageBox::critical(nullptr, tr("Database Error when getting languages"),
                          query.lastError().text());

  while(query.next())
  {
    languages.append(new QAction(query.value(1).toString(), actionsLanguages));

    if (DataBase::getSetting("current_language") == query.value(0).toInt())
    {
      languages.last()->setChecked(true);
      currentLanguage = languages.last();
    }
  }

  actionsLanguages->setExclusive(true);
  languageMenu->addActions(actionsLanguages->actions());
  connect(actionsLanguages, &QActionGroup::triggered, this,
          &MainWindow::actionLanguage);

  actionSettings.setText(tr("&Settings"));
  actionSettings.setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S));
  actionSettings.setIcon(QIcon(":/icons/settings.ico"));
  connect(&actionSettings, &QAction::triggered, dialogSettings, &QDialog::show);
  menuMain.addAction(&actionSettings);

  menuMain.addSeparator();

  quitAction.setText(tr("&Quit"));
#ifdef Q_OS_WIN
  quitAction.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
#else
  quitAction.setShortcut(QKeySequence::Quit);
#endif
  quitAction.setIcon(QIcon(":/icons/exit.ico"));
  connect(&quitAction, &QAction::triggered, this, &QMainWindow::close);
  menuMain.addAction(&quitAction);

  menuBar()->addMenu(&menuMain);

  menuWords.setTitle(tr("&Words"));

  actionAddWord.setText(tr("&Add Word"));
  QList<QKeySequence> addShortcuts;
  addShortcuts.append(Qt::CTRL + Qt::Key_A);
  addShortcuts.append(Qt::CTRL + Qt::Key_N);
  actionAddWord.setShortcuts(addShortcuts);
  actionAddWord.setIcon(QIcon(":/icons/new.ico"));
  connect(&actionAddWord, &QAction::triggered, dialogAddWord, &QDialog::show);
  menuWords.addAction(&actionAddWord);

  actionRefresh.setText(tr("&Refresh"));
  actionRefresh.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  actionRefresh.setIcon(QIcon(":/icons/refresh.ico"));
  connect(&actionRefresh, &QAction::triggered, currentModel,
          &QSqlTableModel::select);
  menuWords.addAction(&actionRefresh);

  menuWords.addSeparator();

  actionfilterWord.setText(tr("&Filter Records"));
  actionfilterWord.setShortcut(QKeySequence::Find);
  actionfilterWord.setIcon(QIcon(":/icons/filter.ico"));
  connect(&actionfilterWord, &QAction::triggered, dialogfilterWord,
          &QDialog::show);
  menuWords.addAction(&actionfilterWord);

  menuMasteredFilter.setTitle(tr("Filter by Mastered"));
  menuWords.addMenu(&menuMasteredFilter);

  actionMastered.setText(tr("Mastered"));
  actionMastered.setCheckable(true);
  actionMastered.setChecked(true);
  connect(&actionMastered, &QAction::toggled, this, &MainWindow::onMasteredToggle);

  actionUnmastered.setText(tr("Unmastered"));
  actionUnmastered.setCheckable(true);
  actionUnmastered.setChecked(true);
  connect(&actionUnmastered, &QAction::toggled, this,
          &MainWindow::onUnmasteredToggle);

  menuMasteredFilter.addAction(&actionMastered);
  menuMasteredFilter.addAction(&actionUnmastered);

  menuDefinedFilter.setTitle(tr("Filter by Defined"));
  menuWords.addMenu(&menuDefinedFilter);

  actionDefined.setText(tr("Defined"));
  actionDefined.setCheckable(true);
  actionDefined.setChecked(true);
  connect(&actionDefined, &QAction::toggled, this, &MainWindow::onDefinedToggle);

  actionUndefined.setText(tr("Undefined"));
  actionUndefined.setCheckable(true);
  actionUndefined.setChecked(true);
  connect(&actionUndefined, &QAction::toggled, this,
          &MainWindow::onUndefinedToggle);

  menuDefinedFilter.addAction(&actionDefined);
  menuDefinedFilter.addAction(&actionUndefined);

  actionGotoWord.setText(tr("&Goto Word"));
  actionGotoWord.setShortcut(QKeySequence::FindNext);
  actionGotoWord.setIcon(QIcon(":/icons/goto.ico"));
  connect(&actionGotoWord, &QAction::triggered, dialogGotoWord, &QDialog::show);
  menuWords.addAction(&actionGotoWord);

  menuWords.addSeparator();

  actionDeleteRecord.setText(tr("&Delete Record"));
  actionDeleteRecord.setShortcut(QKeySequence::Delete);
  actionDeleteRecord.setIcon(QIcon(":/icons/delete.ico"));
  actionDeleteRecord.setEnabled(false);
  connect(&actionDeleteRecord, &QAction::triggered, this,
          &MainWindow::confirmDeleteItem);
  menuWords.addAction(&actionDeleteRecord);

  menuBar()->addMenu(&menuWords);
}

void MainWindow::updateUrls()
{
  urls.clear();

  QSqlQuery query(DataBase::getDb());

  query.prepare("select url from urls where enabled = 1 and language_fk = ? "
                "order by id");
  query.addBindValue(DataBase::getSetting("current_language"));

  if(!query.exec())
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    urls.append(query.value(0).toString());
}

void MainWindow::addWebViews()
{
  for(int i = 0; i < urls.count() - currentWebViews; i++)
  {
    auto *webView = new QWebEngineView(&centreWidget);

    webView->setMinimumHeight(550);

    glCentralGrid.addWidget(webView, row, col);

    webViewList.append(webView);

    if(col == 0)
    {
      col = 1;
    }
    else
    {
      row++;
      col = 0;
    }
  }

  if (urls.count() > currentWebViews)
    currentWebViews = urls.count();
}

void MainWindow::webSearchWord(const QString &word)
{
  if (senderWebView)
  {
    disconnect(senderWebView, &QWebEngineView::loadFinished, this,
               &MainWindow::enaGeInput);
    searchWord = "";
  }

  QString url;
  for(int i = 0; i < urls.size(); i++)
  {
    url = urls[i];

    auto webView = webViewList.at(i);

    if (url.contains("ena.ge"))
    {
      senderWebView = webView;
      searchWord = word;
      connect(senderWebView, &QWebEngineView::loadFinished, this,
              &MainWindow::enaGeInput);
    }

    webView->setUrl(QUrl(url.replace("{word}", word)));
  }
}

void MainWindow::clearBrowsers()
{
  for(int i = 0; i < urls.size(); i++)
    webViewList.at(i)->setUrl(QUrl(""));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  double height = static_cast<double>(this->height()) * 0.5;

  tableWords->setMinimumHeight(static_cast<int>(height));

  double posX = static_cast<double>(this->width()) * 0.32;
  double posY = static_cast<double>(this->height()) * 0.5;

  dialogAddWord->move(static_cast<int>(posX), static_cast<int>(posY));
}

void MainWindow::resizeRowsColumns()
{
  double width = static_cast<double>(centreWidget.width()) * 0.5;

  tableWords->setColumnWidth(4, static_cast<int>(width));

  tableWords->resizeRowsToContents();

  for (int i = 0; i <= modelWords->columnCount(); ++i)
    if(i != 4)
      tableWords->resizeColumnToContents(i);
}

void MainWindow::onSelectionChanged(const QItemSelection &selected,
                                    const QItemSelection &deselected)
{
  auto indices = selected.indexes();
  auto previndices = deselected.indexes();

  if(indices.count() > 0)
  {
    if((previndices.count() == 0 || (previndices[0].row() != indices[0].row() ||
                                     previndices[0].column() != 1)) &&
       indices[0].column() == 1)
    {
      QModelIndex i = currentModel->index(indices[0].row(), 1);

      webSearchWord(currentModel->data(i).toString());
    }

    actionDeleteRecord.setEnabled(true);
  }
  else
  {
    actionDeleteRecord.setEnabled(false);
  }
}

void MainWindow::propagateTabIndex(int index)
{
  dialogAddWord->setCurrentTab(index);

  if (!index)
  {
    currentModel = modelWords;
    currentView = tableWords;

    dialogGotoWord->onTabChange(modelWords, tableWords, index);
    dialogfilterWord->onTabChange(modelWords, tableWords, index);
  }
  else {
    currentModel = &modelIdioms;
    currentView = &tableIdioms;

    dialogGotoWord->onTabChange(&modelIdioms, &tableIdioms, index);
    dialogfilterWord->onTabChange(&modelIdioms, &tableIdioms, index);
  }
}

void MainWindow::onMasteredToggle(const bool &b)
{
  if(!b && !actionUnmastered.isChecked())
    actionUnmastered.setChecked(true);

  applyFilter();
}

void MainWindow::onUnmasteredToggle(const bool &b)
{
  if(!b && !actionMastered.isChecked())
    actionMastered.setChecked(true);

  applyFilter();
}

void MainWindow::onDefinedToggle(const bool &b)
{
  if(!b && !actionUndefined.isChecked())
    actionUndefined.setChecked(true);

  applyFilter();
}

void MainWindow::onUndefinedToggle(const bool &b)
{
  if(!b && !actionDefined.isChecked())
    actionDefined.setChecked(true);

  applyFilter();
}

void MainWindow::applyFilter()
{
  QString filterStr = "";
  QString first = "";
  QString second = "";

  if(actionMastered.isChecked() && !actionUnmastered.isChecked())
    first = "mastered = 1";
  else if(!actionMastered.isChecked() && actionUnmastered.isChecked())
    first = "mastered = 0";

  if(actionDefined.isChecked() && !actionUndefined.isChecked())
    second = "definition != ''";
  else if(!actionDefined.isChecked() && actionUndefined.isChecked())
    second = "definition = ''";

  if(first != "" && second == "")
    filterStr = first;
  else if(first == "" && second != "")
    filterStr = second;
  else if(first != "" && second != "")
    filterStr = first + " and " + second;
  else
    filterStr = "";

  if (!filterStr.isEmpty())
    filterStr += " and ";
  filterStr += "language_fk = " + QString::number(
                  DataBase::getSetting("current_language"));

  modelWords->setFilter(filterStr);
}

void MainWindow::confirmDeleteItem()
{
  int r = currentView->currentIndex().row();
  QModelIndex idx = currentModel->index(r, 1);
  QString word = currentModel->data(idx).toString();

  if(confirmDelete)
  {
    dialogConfirmDelete->setWord(word);

    if(dialogConfirmDelete->exec())
    {
      deleteRecord(r);
    }
  }
  else
  {
    deleteRecord(r);
  }
}

void MainWindow::deleteRecord(int row)
{
  currentModel->removeRow(row);
  currentModel->select();

  if(statusBar()->isVisible())
    updateWordCount();
}

void MainWindow::updateWordCount()
{
  if (!tabWidget.currentIndex())
    threadWordCount->start();

  updateModel();
}

void MainWindow::onBeforeUpdate(int row, QSqlRecord &record)
{
  Q_UNUSED(row)

  QString dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

  record.setValue("updatetime", dateTime);
  record.setGenerated("updatetime", true);

  auto word = record.value("word").toString();

  // Check if already exists
  QSqlQuery query(DataBase::getDb());

  query.prepare("select word from words where word = ?");
  query.addBindValue(word);

  if(!query.exec())
    QMessageBox::critical(nullptr, tr("Database Error while checking word "
                                      "duplication"), query.lastError().text());

  while(query.next())
  {
    currentModel->setData(currentModel->index(row, 1),
                          word + " ");
    currentModel->setData(currentModel->index(row, 2), "Exists!!!");
  }

  threadWordCount->start();
}

void MainWindow::toggleConfirmDelete(bool ok)
{
  confirmDelete = ok;
}

void MainWindow::toggleStatusBar(bool ok)
{
  if(ok)
  {
    statusBar()->show();
  }
  else
  {
    statusBar()->hide();
  }
}

void MainWindow::updateModel()
{
  if (tabWidget.currentIndex())
    modelIdioms.select();
  else
    modelWords->select();
}

void MainWindow::actionLanguage(const QAction *action)
{
  QSqlQuery query(DataBase::getDb());

  query.prepare("select id from languages where language = ?");
  query.addBindValue(action->text());

  if(!query.exec())
    QMessageBox::critical(nullptr, tr("Database Error in getting language id"),
                          query.lastError().text());

  while(query.next())
  {
    int languageID = query.value(0).toInt();

    DataBase::updateSetting("current_language", languageID);
    modelWords->setFilter("language_fk = " + QString::number(languageID));
    modelIdioms.setFilter("language_fk = " + QString::number(languageID));

    updateUrls();
    addWebViews();

    threadWordCount->start();

    currentLanguage = const_cast<QAction*>(action);
  }
}

void MainWindow::onChangeTabKey()
{
  tabWidget.setCurrentIndex(!tabWidget.currentIndex());
}

void MainWindow::onNextLanguage()
{
  for (auto i = languages.begin(); i != languages.end(); ++i)
  {
    auto next = i + 1;

    if ((*i) == currentLanguage && next != languages.end())
    {
      (*next)->trigger();

      return;
    }
  }

  languages.first()->trigger();
}

void MainWindow::onPrevLanguage()
{
  for (auto i = languages.end() - 1; i != languages.begin() - 1; --i)
  {
    auto prev = i - 1;

    if ((*i) == currentLanguage && prev != languages.begin() - 1)
    {
      (*prev)->trigger();

      return;
    }
  }

  languages.last()->trigger();
}

void MainWindow::enaGeInput(bool ok)
{
  if (ok)
  {
    senderWebView->page()->runJavaScript(
          "document.getElementById('word_metauri').value='" + searchWord + "'",
          [](const QVariant &v) {
      Q_UNUSED(v)
      senderWebView->page()->runJavaScript(
            "document.getElementById('submit').click()");
    });
  }
}

void WordCount::run()
{
  QThread::sleep(1);

  int words = 0;
  int defined = 0;
  int undefined = 0;
  int mastered = 0;
  int unmastered = 0;

  QSqlQuery query(DataBase::getDb());

  query.prepare("select count(1) from words where language_fk = ?");
  query.addBindValue(DataBase::getSetting("current_language"));

  if(!query.exec())
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    words = query.value(0).toInt();

  query.prepare("select count(1) from words where language_fk = ? and "
                "definition != ''");
  query.addBindValue(DataBase::getSetting("current_language"));

  if(!query.exec())
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    defined = query.value(0).toInt();

  query.prepare("select count(1) from words where language_fk = ? and "
                "mastered = 1");
  query.addBindValue(DataBase::getSetting("current_language"));

  if(!query.exec())
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    mastered = query.value(0).toInt();

  undefined = words - defined;
  unmastered = words - mastered;

  QStringList statList;
  statList.append("Words:");
  statList.append(QString::number(words));
  statList.append("   Defined:");
  statList.append(QString::number(defined));
  statList.append("   Undefined:");
  statList.append(QString::number(undefined));
  statList.append("   Mastered:");
  statList.append(QString::number(mastered));
  statList.append("   Unmastered:");
  statList.append(QString::number(unmastered));

  bar->setText(statList.join(' '));
}
