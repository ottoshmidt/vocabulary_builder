#include "database.h"

#include <QMessageBox>
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DataBase::db = QSqlDatabase::addDatabase("QSQLITE");

DataBase::DataBase(QObject *parent) : QObject(parent)
{
  setup();
}

DataBase::~DataBase()
{
  db.close();
}

void DataBase::setup()
{
  if(db.isOpen())
    return;

  QFile file("dbase");
  bool exists = false;

  if(file.exists())
    exists = true;

  db.setDatabaseName("dbase");
  if (!db.open())
    QMessageBox::critical(nullptr, "Database Error", db.lastError().text());

  if(!exists)
    createTables();
}

void DataBase::createTables()
{
  QSqlQuery query(db);

  QString dbErrorStr = tr("Database Error while creating tables.");

  if(!query.exec("CREATE TABLE words(id integer primary key autoincrement,"
                 "word varchar(30) unique, note varchar(10), pronunciation "
                 "varchar(35),definition varchar(150), rating int, mastered "
                 "int(1) default 0, "
                 "timestamp date default (datetime('now','localtime')))"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("CREATE TABLE languages(id INTEGER primary key autoincrement, "
                 "language varchar(20))"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("INSERT INTO languages(id, language) VALUES(1, 'English')"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("CREATE TABLE urls(id integer primary key autoincrement, "
                 "url varchar(100) unique, enabled int(1), language_fk integer, "
                 "foreign key (language_fk) references languages(id))"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("INSERT INTO urls(url, enabled, language_fk) "
                 "VALUES('https://www.merriam-webster.com/dictionary/{word}', 1, 1),"
                 "('https://en.oxforddictionaries.com/definition/{word}', 1, 1),"
                 "('https://www.google.ge/search?q={word}&tbm=isch', 1, 1),"
                 "('https://en.wikipedia.org/wiki/{word}', 1, 1)"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("CREATE TABLE settings(id integer primary key autoincrement, "
                 "key varchar(40), value int, value_str varchar(25))"))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());

  if(!query.exec("INSERT INTO settings (key, value) "
                 "VALUES"
                 "('alternating_row_colors', 0),"
                 "('inwords', 1),"
                 "('indefinitions', 0),"
                 "('exactmatch', 0),"
                 "('confirm_delete', 1),"
                 "('status_bar', 1)"
                 ))
    QMessageBox::critical(nullptr, dbErrorStr,
                          query.lastError().text());
}

QSqlDatabase DataBase::getDb()
{
  if(!db.isOpen())
    setup();

  return db;
}

void DataBase::updateSetting(const QString &param, const QVariant &value)
{
  QSqlQuery query(db);

  QString col = "value";

  if(value.type() == QVariant::String)
    col = "value_str";

  query.prepare("update settings set " + col + " = ? where key = ?");
  query.addBindValue(value);
  query.addBindValue(param);

  if(!query.exec())
    QMessageBox::critical(nullptr, tr("Database Error while updating settings"),
                          query.lastError().text());
}

bool DataBase::getSetting(const QString &param)
{
  QSqlQuery query(db);
  bool res = false;

  query.prepare("select value from settings where key = ?");
  query.addBindValue(param);

  if(!query.exec())
    QMessageBox::critical(nullptr, tr("Database Error while updating settings"),
                          query.lastError().text());


  while(query.next())
    res = query.value(0).toBool();

  return res;
}

