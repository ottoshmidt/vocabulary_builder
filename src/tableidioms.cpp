#include "mainwindow.h"
#include "tableidioms.h"

TableIdioms::TableIdioms()
{
  setSortingEnabled(true);
  sortByColumn(7, Qt::DescendingOrder);
  setItemDelegateForColumn(4, new CheckBoxDelegate(this));
}

bool TableIdioms::viewportEvent(QEvent *e)
{
  QAbstractItemView::viewportEvent(e);

  setColumnWidth(1, width() / 5);
  setColumnWidth(2, width() / 2.16);

  resizeRowsToContents();

  for (int i = 0; i <= model()->columnCount(); ++i)
    if(i != 1 && i != 2)
      resizeColumnToContents(i);

  return true;
}

void TableIdioms::selectionChanged(const QItemSelection &selected,
                                   const QItemSelection &deselected)
{
  emit selectionChangedSig(selected, deselected);
}
