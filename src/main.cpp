#include "mainwindow.h"
#include "database.h"

#include <QApplication>
#include <QStyleFactory>

#include <QMessageBox>
#include <QSqlDatabase>


void setupStyle(QApplication &app)
{
  // Set the application style to "Fusion"
  app.setStyle(QStyleFactory::create("Fusion"));

  // Set up a dark palette
  QPalette darkPalette;
  darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::WindowText, QColor(0x35, 0x84, 0xE4));
  darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
  darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ToolTipBase, QColor(0x35, 0x84, 0xE4));
  darkPalette.setColor(QPalette::ToolTipText, QColor(0x35, 0x84, 0xE4));
  darkPalette.setColor(QPalette::Text, QColor(0x35, 0x84, 0xE4));
  darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::ButtonText, QColor(0x35, 0x84, 0xE4));
  darkPalette.setColor(QPalette::BrightText, Qt::red);
  darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  darkPalette.setColor(QPalette::HighlightedText, Qt::black);

  // Apply the palette
  app.setPalette(darkPalette);

  // Apply a dark stylesheet
  // app.setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  a.setApplicationName("Vocabulary Builder");
  a.setApplicationVersion("0.5.9");

  setupStyle(a);

  DataBase dbase;
  Q_UNUSED(dbase)


  MainWindow w;
  w.showMaximized();

  return a.exec();
}
