#ifndef DIALOGCONFIRMDELETE_H
#define DIALOGCONFIRMDELETE_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QShowEvent>

class DialogConfirmDelete : public QDialog
{
public:
  DialogConfirmDelete(QWidget *parent = nullptr);

private:
  QLabel label;
  QCheckBox cbDontAsk;
  QPushButton pbOK;
  QPushButton pbCancel;
  QGridLayout glCentralGrid;

  bool firstRun;

  void setupWidgets();
  void setupLayout();

  void showEvent(QShowEvent *event) override;

private slots:
  void onDontAskToggle(const bool &checked);

public:
  void setWord(const QString &word);
};

#endif // DIALOGCONFIRMDELETE_H
