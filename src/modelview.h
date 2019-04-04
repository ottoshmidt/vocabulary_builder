#ifndef MODELVIEW_H
#define MODELVIEW_H

#include "custommodel.h"
#include "customtableview.h"
#include "checkboxdelegate.h"

class ModelView : public QObject
{
  Q_OBJECT

public:
  explicit ModelView(QObject *parent = nullptr);
  ~ModelView();

  static CustomModel *model();
  static CustomTableView *view();

private:
  static CustomModel *modelWords;
  static CustomTableView *tableWords;
  CheckBoxDelegate *cbDelegate;

  void setupModel();
  void setupView();
};

#endif // MODELVIEW_H
