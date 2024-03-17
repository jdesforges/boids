#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "keys.h"
#include "renderarea.h"
#include "spinbox.h"

#include <QApplication>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  setWindowTitle("Boids");

  RenderArea *ra = new RenderArea(this);

  connect(this, &MainWindow::valueChanged, ra, &RenderArea::handleValueChanged);
  connect(this, &MainWindow::setBoids, ra, &RenderArea::setBoids);
  connect(ra, &RenderArea::resized, this, &MainWindow::resized);
  connect(ui->drawNeighborhood, &QCheckBox::stateChanged, ra,
          &RenderArea::handleDrawNeighborhood);

  ui->centralwidget->layout()->addWidget(ra);

  auto const setupSpinBox = [this, ra](QString const &name_, int min_, int max_,
                                       int default_, bool balance_ = false) {
    SpinBox *s = new SpinBox(name_, min_, max_, default_, balance_, this);
    connect(
        s, &SpinBox::valueChanged, this,
        [this, name_](int val_) {
          if (RULES.count(name_)) {
            balanceSpinBoxes(name_);
          }

          emit valueChanged(name_, val_);
        },
        Qt::QueuedConnection);

    QGridLayout *layout = dynamic_cast<QGridLayout *>(ui->controls->layout());
    layout->addWidget(s, layout->rowCount(), 0, 1, -1, Qt::AlignLeft);

    // Emit initial configs on next cycle
    QTimer::singleShot(0, this, [this, name_, default_]() {
      emit valueChanged(name_, default_);
    });
  };

  for (auto const &rule : RULES) {
    setupSpinBox(rule, 0, 100, 100, true);
  }

  setupSpinBox(BOIDS, 0, 5000, 200);
  setupSpinBox(NEIGHBORHOOD, 0, 1000, 200);

  balanceSpinBoxes("");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::balanceSpinBoxes(QString const &name_) {
  double sum = 0;

  double fixedValue = 0;
  for (size_t i = 0; i < ui->controls->layout()->count(); ++i) {
    SpinBox *spinBox =
        dynamic_cast<SpinBox *>(ui->controls->layout()->itemAt(i)->widget());

    if (!spinBox || !spinBox->shouldBalance()) {
      continue;
    }

    if (spinBox->name() == name_) {
      fixedValue = spinBox->value();
    }

    sum += double(spinBox->value());
  }

  if (sum <= 100) {
    return;
  }

  for (size_t i = 0; i < ui->controls->layout()->count(); ++i) {
    SpinBox *spinBox =
        dynamic_cast<SpinBox *>(ui->controls->layout()->itemAt(i)->widget());

    if (!spinBox || !spinBox->shouldBalance() || spinBox->name() == name_) {
      continue;
    }

    spinBox->setValue((100 - fixedValue) * spinBox->value() /
                      (sum - fixedValue));
  }
}
