#ifndef TABLEIDIOMS_H
#define TABLEIDIOMS_H

#include <QTableView>

class TableIdioms : public QTableView
{
  Q_OBJECT

public:
  TableIdioms();

private:
  bool viewportEvent(QEvent *e) override;

  void selectionChanged(const QItemSelection &selected,
                        const QItemSelection &deselected) override;

signals:
  void selectionChangedSig(const QItemSelection &selected,
                           const QItemSelection &deselected);
};

#endif // TABLEIDIOMS_H
