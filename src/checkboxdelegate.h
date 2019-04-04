#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QStyledItemDelegate>

class CheckBoxDelegate : public QStyledItemDelegate
{
public:
  CheckBoxDelegate(QWidget *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
//  QSize sizeHint(const QStyleOptionViewItem &option,
//                 const QModelIndex &index) const override;
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
//  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
//  void setModelData(QWidget *editor, QAbstractItemModel *model,
//                    const QModelIndex &index) const override;
  void updateEditorGeometry(QWidget *editor,
           const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private slots:
  void commitAndCloseEditor();
};

#endif // CHECKBOXDELEGATE_H
