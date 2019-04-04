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

  connect(&pbGotoWord, &QPushButton::released, this, &DialogGotoWord::onGoPress);
  connect(&pbClose, &QPushButton::released, this, &QDialog::close);
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
  int i = 0;
  bool found = false;

  QSqlRecord rec = ModelView::model()->record(0);

  while(!rec.isNull("word"))
  {
    QString w = rec.value("word").toString();

    if(w == word)
    {
      ModelView::view()->setCurrentIndex(ModelView::model()->index(i, 0));
      ModelView::model()->selectRow(i);

      found = true;
      lbNotFound.clear();

      break;
    }

    rec = ModelView::model()->record(++i);
  }

  QString txt = "Word '" + word + "' not found";

  if(!found)
    lbNotFound.setText(tr(txt.toStdString().c_str()));
}

void DialogGotoWord::onGoPress()
{
  gotoWord(leWord.text());
}
