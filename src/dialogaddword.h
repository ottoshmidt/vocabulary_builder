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

private:
  QLineEdit leWord;
  QLabel lbResult;
  QLabel lbGoToWord;
  QGridLayout gridLayout;
  QPushButton pbAddWord;
  QPushButton pbCloseWindow;

  void hideEvent(QHideEvent *event);

private slots:
  void addWord();
  void gotoWord(const QString &link);

signals:
  void linkClicked(QString word);
  void wordInserted();
  void wordUpdated();
};

#endif // ADDWORD_H
