#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "simulation.h"

#include <QMainWindow>
#include <QResizeEvent>
#include <QString>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/// \brief The main window of the application. Hosts the config interface and
/// boids rendering.
class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

signals:
  void resized(QResizeEvent *event_);

  void setBoids(std::vector<boid> const &boids_);

  /// \brief Signal emitted when a config value has changed
  /// \param name_ Name of the config item
  /// \param val_ New config value
  void valueChanged(QString const &name_, int val_);

private:
  /// \brief Balance all spinboxes
  /// \param name_ Name of spinBox to ignore for balancing
  void balanceSpinBoxes(QString const &name_);

  Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
