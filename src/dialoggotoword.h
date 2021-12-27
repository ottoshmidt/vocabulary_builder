#ifndef GOTOWORD_H
#define GOTOWORD_H

#include <QDialog>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QHideEvent>
#include <QSqlTableModel>
#include <QTableView>

class DialogGotoWord : public QDialog
{
  Q_OBJECT

public:
  DialogGotoWord(QWidget *parent = nullptr);

  void onTabChange(QSqlTableModel *model, QTableView *view, int currentTab);
private:
  QGridLayout glfilterWord;
  QLineEdit leWord;
  QPushButton pbGotoWord;
  QPushButton pbClose;
  QLabel lbNotFound;

  QSqlTableModel *model;
  QTableView *view;

  int currentTab;
  int recordCount;

  void setupWidgets();
  void setupLayout();

  void hideEvent(QHideEvent *event) override;

public slots:
  void gotoWord(const QString &word);
  void onGoPress();
};

#endif // GOTOWORD_H
