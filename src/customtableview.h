#ifndef CUSTOMTABLEVIEW_H
#define CUSTOMTABLEVIEW_H

#include <QTableView>
#include <QTableWidget>

class CustomTableView : public QTableView
{
  Q_OBJECT

public:
  CustomTableView(QWidget *parent = nullptr);

signals:
  void viewPortChanged();
  void selectionChangedSig(const QItemSelection &selected,
                           const QItemSelection &deselected);
private:
  bool viewportEvent(QEvent *e) override;
  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected) override;
};

#endif // CUSTOMTABLEVIEW_H
