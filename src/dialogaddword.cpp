#include "dialogaddword.h"
#include "database.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMessageBox>

DialogAddWord::DialogAddWord(QWidget *parent)
{
  setParent(parent);
  setWindowFlags(Qt::Dialog);

  setWindowTitle(tr("Add New Word"));
  setMinimumSize(400, 140);

  pbAddWord.setText(tr("Add Word"));
  pbClose.setText(tr("Close"));

  gridLayout.addWidget(&leWord, 0, 0, 1, 3);
  gridLayout.addWidget(&lbResult, 1, 0, 1, 2);
  gridLayout.addWidget(&lbGoToWord, 1, 2);
  gridLayout.addWidget(&pbAddWord, 3, 1);
  gridLayout.addWidget(&pbClose, 3, 2);
  gridLayout.setRowStretch(2, 1);
  gridLayout.setColumnStretch(0, 1);
  gridLayout.setSpacing(3);

  setLayout(&gridLayout);

  connect(&pbClose, &QPushButton::released, this, &QWidget::close);
  connect(&pbAddWord, &QPushButton::released, this, &DialogAddWord::addWord);
  connect(&lbGoToWord, &QLabel::linkActivated, this, &DialogAddWord::gotoWord);
}

void DialogAddWord::hideEvent(QHideEvent *event)
{
  leWord.clear();
  lbResult.clear();
  lbGoToWord.clear();

  leWord.setFocus();

  event->accept();
}

void DialogAddWord::setCurrentTab(int currentTab)
{
  this->currentTab = currentTab;
}

void DialogAddWord::addWord()
{
  QSqlQuery query(DataBase::getDb());

  int rating = 0;
  QString definition;
  QString word = leWord.text();

  if(word.isEmpty())
    return;

  query.prepare("SELECT rating, definition FROM words WHERE word = ?");
  query.addBindValue(word);

  if(!query.exec())
    QMessageBox::critical(nullptr, "Database Error", query.lastError().text());

  while(query.next())
  {
    rating = query.value(0).toInt();
    definition = query.value(1).toString();
  }

  query.clear();

  if(rating > 0)
  {
    query.prepare("UPDATE words SET "
                  " rating = ?, "
                  " updatetime = datetime('now','localtime') "
                  "WHERE word = ?");
    query.addBindValue(++rating);
    query.addBindValue(word);

    if(query.exec())
    {
      QString defined = "Undefined";

      if(definition != "")
        defined = "Defined";

      lbResult.setText("Word '" + word + "' exists! (" + defined + ") Rating: " +
                       QString::number(rating));

      lbGoToWord.setText("<a href=\""+ word +"\">Go to word</a>");

      emit wordUpdated();
    }
    else
    {
      QMessageBox::critical(nullptr, "Database Error", query.lastError().text());
    }
  }
  else
  {
    QString tableStr;
    if (!currentTab)
      tableStr = "words(word";
    else
      tableStr = "idioms(idiom";

    query.prepare("INSERT INTO "+tableStr+", rating, language_fk) values(?,?,?)");
    query.addBindValue(word);
    query.addBindValue(1);
    query.addBindValue(DataBase::getSetting("current_language"));

    if(query.exec())
    {
      lbResult.setText("Word '" + word + "' added. Rating: 1");

      lbGoToWord.clear();

      emit wordInserted();
    }
    else
    {
      QMessageBox::critical(nullptr, "Database Error", query.lastError().text());
    }
  }

  leWord.clear();
}

void DialogAddWord::gotoWord(const QString &word)
{
  emit linkClicked(word);
}
