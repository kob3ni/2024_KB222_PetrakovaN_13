#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.move( QApplication::screens().first()->geometry().center() - w.rect().center() );

  w.show();
  return a.exec();
}
