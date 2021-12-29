#include "dialoggotoword.h"
#include "modelview.h"

#include <QDebug>
#include <QSqlRecord>

DialogGotoWord::DialogGotoWord(QWidget *parent)
{
  setParent(parent);
  setWindowFlags(Qt::Dialog);
  setWindowTitle(tr("Goto Word"));
  setMinimumSize(340, 130);

  setupWidgets();
  setupLayout();

  recordCount = 0;

  connect(&pbGotoWord, &QPushButton::released, this, &DialogGotoWord::onGoPress);
  connect(&pbClose, &QPushButton::released, this, &QDialog::close);
}

void DialogGotoWord::onTabChange(QSqlTableModel *model, QTableView *view,
                                 int currentTab)
{
  this->model = model;
  this->view = view;
  this->currentTab = currentTab;
  recordCount = 0;
}

void DialogGotoWord::setupWidgets()
{
  pbClose.setText(tr("&Close"));
  pbGotoWord.setText(tr("&Go"));
}

void DialogGotoWord::setupLayout()
{
  glfilterWord.addWidget(&leWord, 0, 0, 1, 4);
  glfilterWord.addWidget(&lbNotFound, 1, 0, 1, 2);
  glfilterWord.addWidget(&pbGotoWord, 3, 2);
  glfilterWord.addWidget(&pbClose, 3, 3);
  glfilterWord.setRowStretch(2, 1);

  setLayout(&glfilterWord);

  leWord.setFocus();
}

void DialogGotoWord::hideEvent(QHideEvent *event)
{
  leWord.setFocus();
  leWord.clear();
  lbNotFound.clear();

  event->accept();
}

void DialogGotoWord::gotoWord(const QString &word)
{
  if (word.isEmpty())
    return;

  view->setCurrentIndex(model->index(0, 0));

  bool found = false;
  int startingPoint = recordCount;

  QSqlRecord rec = model->record(recordCount);

  QString colName;
  if (!currentTab)
    colName = "word";
  else
    colName = "idiom";

  while(!rec.isNull(colName))
  {
    QString w = rec.value(colName).toString();

    if(w.contains(word.toLower()))
    {
      view->setCurrentIndex(model->index(recordCount, 0));

      found = true;
      lbNotFound.clear();
      recordCount++;

      break;
    }

    rec = model->record(++recordCount);

    // Continue from the beginning
    if (rec.isNull(colName) && startingPoint > 0)
    {
      recordCount = 0;
      startingPoint = 0;
      rec = model->record(recordCount);
    }
  }

  QString txt = "Word '" + word + "' not found";

  if(!found)
  {
    lbNotFound.setText(tr(txt.toStdString().c_str()));
    view->setCurrentIndex(model->index(-1, -1));
    recordCount = 0;
  }
}

void DialogGotoWord::onGoPress()
{
  gotoWord(leWord.text());
}
