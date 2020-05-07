#include "mainwindow.h"
#include "database.h"

#include <QApplication>

#include <QMessageBox>
#include <QSqlDatabase>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("Vocabulary Builder");
  a.setApplicationVersion("0.5.0");

  DataBase dbase;
  Q_UNUSED(dbase)


  MainWindow w;
  w.showMaximized();

  return a.exec();
//  return 0;
}
