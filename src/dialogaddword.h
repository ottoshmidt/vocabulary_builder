#ifndef ADDWORD_H
#define ADDWORD_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QCloseEvent>

class DialogAddWord : public QDialog
{
  Q_OBJECT

public:
  DialogAddWord(QWidget *parent = nullptr);

  void setCurrentTab(int currentTab);

private:
  QLineEdit leWord;
  QLabel lbResult;
  QGridLayout gridLayout;
  QPushButton pbAddWord;
  QPushButton pbCloseWindow;

  int currentTab;

  void hideEvent(QHideEvent *event) override;


private slots:
  void addWord();

signals:
  void wordInserted();
  void wordUpdated();
};

#endif // ADDWORD_H
