#include "checkboxdelegate.h"

#include <QApplication>
#include <QCheckBox>
#include <QPainter>
#include <QDebug>

CheckBoxDelegate::CheckBoxDelegate(QWidget *parent) :
  QStyledItemDelegate(parent)
{
}

void CheckBoxDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
  QStyleOptionButton butt;

  QRect oRect1 = option.rect;

  QRect oRect(oRect1.x() + 34, oRect1.y(), oRect1.width(), oRect1.height());
  butt.rect = oRect;
  butt.state = index.data().toBool() ? QStyle::State_On : QStyle::State_Off;

  QApplication::style()->drawControl(QStyle::CE_CheckBox, &butt, painter);
}

QWidget *CheckBoxDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &/*option*/,
                                        const QModelIndex &/*index*/) const
{
  QCheckBox *box = new QCheckBox(parent);

  QPalette p(Qt::white);
  box->setPalette(p);
  box->setAutoFillBackground(true);

  return box;
}

void CheckBoxDelegate::updateEditorGeometry(QWidget *editor,
                                            const QStyleOptionViewItem &option,
                                            const QModelIndex &/*index*/) const
{
  QRect rOld = option.rect;
  QRect rNew(rOld.x() + 23, rOld.y(), rOld.width(), rOld.height());
  editor->setGeometry(rNew);
}
