#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QObject>

class DataBase : public QObject
{
  Q_OBJECT

public:
  DataBase(QObject *parent = nullptr);
  ~DataBase();

  static void setup();
  static QSqlDatabase getDb();
  static void updateSetting(const QString &param, const QVariant &value);
  static bool getSetting(const QString &param);

private:
  static QSqlDatabase db;
  static void createTables();
};

#endif // DATABASE_H
