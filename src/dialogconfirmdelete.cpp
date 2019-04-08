#include "dialogconfirmdelete.h"
#include "database.h"

#include <QVariant>

DialogConfirmDelete::DialogConfirmDelete(QWidget *parent)
{
  setParent(parent);
  setWindowTitle(tr("Delete Record"));
  setWindowFlags(Qt::Dialog);
  setMinimumSize(290, 130);

  setupWidgets();
  setupLayout();
}

void DialogConfirmDelete::setupWidgets()
{
  label.setText(tr("Are you sure you want to delete word ?"));

  cbDontAsk.setText(tr("Don't ask again"));
  cbDontAsk.setChecked(!DataBase::getSetting("confirm_delete"));

  pbOK.setText(tr("OK"));
  pbOK.setDefault(true);
  pbCancel.setText(tr("Cancel"));

  connect(&cbDontAsk, &QCheckBox::toggled, this,
          &DialogConfirmDelete::onDontAskToggle);
  connect(&pbOK, &QPushButton::released, this, &QDialog::accept);
  connect(&pbCancel, &QPushButton::released, this, &QDialog::reject);
}

void DialogConfirmDelete::setupLayout()
{
  glCentralGrid.addWidget(&label, 0, 0, 1, 3);
  glCentralGrid.addWidget(&cbDontAsk, 1, 0, 1, 2);
  glCentralGrid.setRowStretch(2, 1);
  glCentralGrid.addWidget(&pbOK, 3, 1);
  glCentralGrid.addWidget(&pbCancel, 3, 2);
  setLayout(&glCentralGrid);
}

void DialogConfirmDelete::showEvent(QShowEvent *event)
{
  cbDontAsk.setChecked(false);

  event->accept();
}

void DialogConfirmDelete::onDontAskToggle(const bool &checked)
{
  if(!firstRun)
    DataBase::updateSetting("confirm_delete", !checked);

  extern bool confirmDelete;
  confirmDelete = !checked;

  firstRun = false;
}

void DialogConfirmDelete::setWord(const QString &word)
{
  QString str = "Are you sure you want to delete word '" + word + "'?";
  label.setText(tr(str.toStdString().c_str()));
}
