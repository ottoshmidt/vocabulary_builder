#include "dialogfilter.h"
#include "modelview.h"
#include "database.h"

#include <QDebug>


DialogFilter::DialogFilter(QWidget *parent)
{
  setParent(parent);
  setWindowTitle(tr("Filter Records"));
  setWindowFlags(Qt::Dialog);
  setMinimumSize(340, 130);

  setupWidgets();
  setupLayout();
}

void DialogFilter::onTabChange(QSqlTableModel *model, QTableView *view,
                               int currentTab)
{
  this->model = model;
  this->view = view;
  this->currentTab = currentTab;
}

void DialogFilter::setupWidgets()
{
  cbInWords.setText(tr("In Words"));
  cbInWords.setChecked(true);
  cbInDefinitions.setText(tr("In Definitions"));
  cbInDefinitions.setChecked(false);
  cbExactMatch.setText(tr("Exact Match"));
  cbExactMatch.setChecked(false);

  pbfilterWordClose.setParent(this);
  pbfilterWordClose.setText(tr("&Close"));
  pbfilterWord.setParent(this);
  pbfilterWord.setText(tr("&Filter"));

  setLayout(&glfilterWord);

  connect(&pbfilterWordClose, &QPushButton::released, this, &QDialog::close);

  connect(&pbfilterWord, &QPushButton::released, this,
          &DialogFilter::filterWord);

  pbfilterWord.setDefault(true);

  cbExactMatch.setChecked(DataBase::getSetting("exactmatch"));
  cbInWords.setChecked(DataBase::getSetting("inwords"));
  cbInDefinitions.setChecked(DataBase::getSetting("indefinitions"));

  connect(&cbExactMatch, &QCheckBox::toggled, this, &DialogFilter::switchExactMatch);
  connect(&cbInWords, &QCheckBox::toggled, this, &DialogFilter::switchInWords);
  connect(&cbInDefinitions, &QCheckBox::toggled, this,
          &DialogFilter::switchInDefinitions);
}

void DialogFilter::setupLayout()
{
  glfilterWord.addWidget(&leFilterWord, 0, 0, 1, 4);
  glfilterWord.addWidget(&cbInWords, 1, 0);
  glfilterWord.addWidget(&cbInDefinitions, 1, 1);
  glfilterWord.addWidget(&cbExactMatch, 1, 2);
  glfilterWord.addWidget(&pbfilterWord, 3, 2);
  glfilterWord.addWidget(&pbfilterWordClose, 3, 3);
  glfilterWord.setRowStretch(2, 1);

  leFilterWord.setFocus();
}

void DialogFilter::hideEvent(QHideEvent *event)
{
  leFilterWord.setFocus();

  event->accept();
}

void DialogFilter::filterWord()
{
  QString langFilter = "language_fk = " + QString::number(
                         DataBase::getSetting("current_language")) + " and ";
  QString wordFilter;
  QString defFilter;

  CustomModel * modelWords = ModelView::model();

  if(cbExactMatch.isChecked())
  {
    wordFilter = "word like '" + leFilterWord.text() + "'";
    defFilter = "definition like '" + leFilterWord.text() + "'";

    if(cbInWords.isChecked() && !cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + wordFilter);
    }
    else if(!cbInWords.isChecked() && cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + defFilter);
    }
    else if(cbInWords.isChecked() && cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + " (" + wordFilter + " or " + defFilter
                            + " )");
    }
  }
  else
  {
    wordFilter = "word like '%" + leFilterWord.text() + "%'";
    defFilter = "definition like '%" + leFilterWord.text() + "%'";

    if(cbInWords.isChecked() && !cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + wordFilter);
    }
    else if(!cbInWords.isChecked() && cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + defFilter);
    }
    else if(cbInWords.isChecked() && cbInDefinitions.isChecked())
    {
      modelWords->setFilter(langFilter + " (" + wordFilter + " or " + defFilter
                            + " )");
    }
  }
}

void DialogFilter::switchExactMatch(const bool &checked)
{
  if(!firstRun)
    DataBase::updateSetting("exactmatch", checked);

  firstRun = false;
}

void DialogFilter::switchInWords(const bool &checked)
{
  if(!firstRun)
    DataBase::updateSetting("inwords", checked);

  firstRun = false;
}

void DialogFilter::switchInDefinitions(const bool &checked)
{
  if(!firstRun)
    DataBase::updateSetting("indefinitions", checked);

  firstRun = false;
}
