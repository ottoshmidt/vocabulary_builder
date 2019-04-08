#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QHideEvent>

class DialogFilter : public QDialog
{
public:
  DialogFilter(QWidget *parent = nullptr);

private:
  QLineEdit leFilterWord;
  QCheckBox cbInWords;
  QCheckBox cbInDefinitions;
  QCheckBox cbExactMatch;
  QPushButton pbfilterWord;
  QPushButton pbfilterWordClose;
  QGridLayout glfilterWord;

  void setupWidgets();
  void setupLayout();

  void hideEvent(QHideEvent *event);

  bool firstRun;

public slots:
  void filterWord();

private slots:
  void switchExactMatch(const bool &checked);
  void switchInWords(const bool &checked);
  void switchInDefinitions(const bool &checked);
};

#endif // FILTERDIALOG_H
