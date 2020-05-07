#include "mainwindow.h"
#include "database.h"

#include <QMessageBox>

#include <QCheckBox>
#include <QScrollBar>
#include <QStatusBar>

//TODO: Add New or Filter window is open, switch to other window (alt+tab) switch back, the small window loses focus (annoying).
//TODO: Timestamp so that words with rating >1 can be sorted newest first (IDs only won't work)
//TODO: BUG: Go to word link doesn't work properly when one word is added and then another (existing) one is added.
//TODO: BUG: Find out why Add Word window opens on the first desktop always, and Filter window on the same as the Mainwindow.
//TODO: BUG: when sorted by name and add new word, problems with appearing that word.
//TODO: Idiom support
//TODO: Reflect website modifications in already loaded programme
//TODO: Add multiple language dictionaries
//TODO: Ability to save database in another location (Share drive, Dropbox, etc.)
//TODO: About window
//TODO: pdf, csv, excel exports
//TODO: Db file export/import
//TODO: Delete word on right-click and del button
//TODO: check if we can set mastered checkbox in the middle automatically
//TODO: Translations geo,rus
//TODO: System tray icon (configurable from settings)
//TODO: Configurable keyboard shortcuts

bool showStatusBar;
bool confirmDelete;

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

  connect(dialogAddWord, &DialogAddWord::linkClicked, dialogGotoWord,
          &DialogGotoWord::gotoWord);

  connect(dialogAddWord, &DialogAddWord::wordInserted, this,
          &MainWindow::updateWordCount);

  connect(dialogSettings, &DialogSettings::toggleConfirmDelete, this,
          &MainWindow::toggleConfirmDelete);

  connect(dialogSettings, &DialogSettings::toggleStatusBar, this,
          &MainWindow::toggleStatusBar);
}

void MainWindow::setupModelView()
{
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
}

void MainWindow::setupCentralGrid()
{
  centreWidget.setLayout(&glCentralGrid);

  centralScroll.setWidget(&centreWidget);
  centralScroll.setWidgetResizable(true);

  setCentralWidget(&centralScroll);

  glCentralGrid.addWidget(tableWords, 0, 0, 1, 2);

  QSqlQuery query(DataBase::getDb());

  if(!query.exec("select url from urls where enabled = 1 order by id"))
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    urls.append(query.value(0).toString());

  int row = 1;
  int col = 0;

  for(int i = 0; i < urls.count(); i++)
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

  statBar = new QStatusBar();
  statBar->addWidget(&lbStatBar);
  threadWordCount = new WordCount(&lbStatBar);
  setStatusBar(statBar);
  threadWordCount->start();
  toggleStatusBar(showStatusBar);
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

  actionAddWord.setText(tr("&New Word"));
  actionAddWord.setShortcut(QKeySequence::New);
  actionAddWord.setIcon(QIcon(":/icons/new.ico"));
  connect(&actionAddWord, &QAction::triggered, dialogAddWord, &QDialog::show);
  menuWords.addAction(&actionAddWord);

  actionRefresh.setText(tr("&Refresh"));
  actionRefresh.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  actionRefresh.setIcon(QIcon(":/icons/refresh.ico"));
  connect(&actionRefresh, &QAction::triggered, modelWords, &QSqlTableModel::select);
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
  connect(&actionDeleteRecord, &QAction::triggered, this, &MainWindow::confirmDeleteItem);
  menuWords.addAction(&actionDeleteRecord);

  menuBar()->addMenu(&menuWords);
}

void MainWindow::webSearchWord(const QString &word)
{
  QString url;
  for(int i = 0; i < urls.size(); i++)
  {
    url = urls[i];
    webViewList.at(i)->setUrl(QUrl(url.replace("{word}", word)));
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

  double height = static_cast<double>(this->height()) * 0.46;

  tableWords->setMinimumHeight(static_cast<int>(height));

  resizeRowsColumns();

  double posX = static_cast<double>(this->width()) * 0.32;
  double posY = static_cast<double>(this->height()) * 0.5;

  dialogAddWord->move(static_cast<int>(posX), static_cast<int>(posY));
}

void MainWindow::resizeRowsColumns()
{
  double width = static_cast<double>(centreWidget.width()) * 0.52;

  tableWords->setColumnWidth(4, static_cast<int>(width));

  tableWords->resizeRowsToContents();

  for (int i = 0; i <= 6; ++i)
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
      QModelIndex i = modelWords->index(indices[0].row(), 1);

      webSearchWord(modelWords->data(i).toString());
    }

    actionDeleteRecord.setEnabled(true);
  }
  else
  {
    actionDeleteRecord.setEnabled(false);
  }
}

void MainWindow::openAddWordsWindow()
{
  dialogAddWord->show();
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

  modelWords->setFilter(filterStr);
}

void MainWindow::confirmDeleteItem()
{
  int r = tableWords->currentIndex().row();
  QModelIndex i = modelWords->index(r, 1);
  QString word = modelWords->data(i).toString();

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
  modelWords->removeRow(row);
  modelWords->select();

  if(statusBar()->isVisible())
    updateWordCount();
}

void MainWindow::updateWordCount()
{
  threadWordCount->start();
}

void MainWindow::onBeforeUpdate(int row, QSqlRecord &record)
{
  Q_UNUSED(row)
  Q_UNUSED(record)

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

void WordCount::run()
{
  QThread::sleep(1);

  int words = 0;
  int defined = 0;
  int undefined = 0;
  int mastered = 0;
  int unmastered = 0;

  QSqlQuery query(DataBase::getDb());

  if(!query.exec("select count(1) from words"))
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    words = query.value(0).toInt();

  if(!query.exec("select count(1) from words where definition != ''"))
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
    defined = query.value(0).toInt();

  if(!query.exec("select count(1) from words where mastered = 1"))
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
