#ifndef GOTOWORD_H
#define GOTOWORD_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QHideEvent>

class DialogGotoWord : public QDialog
{
  Q_OBJECT

public:
  DialogGotoWord(QWidget *parent = nullptr);

private:
  QGridLayout glfilterWord;
  QLineEdit leWord;
  QPushButton pbGotoWord;
  QPushButton pbClose;
  QLabel lbNotFound;

  void setupWidgets();
  void setupLayout();

  void hideEvent(QHideEvent *event);

public slots:
  void gotoWord(const QString &word);
  void onGoPress();
};

#endif // GOTOWORD_H
