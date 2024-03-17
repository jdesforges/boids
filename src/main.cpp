#include "mainwindow.h"
#include "simulation.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  MainWindow w;
  w.show();

  simulation sim;

  QObject::connect(&sim, &simulation::update, &w, &MainWindow::setBoids);

  QObject::connect(&w, &MainWindow::resized, &sim, &simulation::handleResized);

  QObject::connect(&w, &MainWindow::valueChanged, &sim,
                   &simulation::handleValueChanged);

  sim.start();

  return a.exec();
}
