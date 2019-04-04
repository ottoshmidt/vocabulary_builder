#include "customtableview.h"

#include <QDebug>

CustomTableView::CustomTableView(QWidget *parent) :
  QTableView(parent)
{

}

bool CustomTableView::viewportEvent(QEvent *e)
{
  QAbstractItemView::viewportEvent(e);

  emit viewPortChanged();

  return true;
}


void CustomTableView::selectionChanged(const QItemSelection &selected,
                                  const QItemSelection &deselected)
{
  emit selectionChangedSig(selected, deselected);
}
