#include "spinBox.h"
#include "ui_spinbox.h"

#include <QLabel>
#include <QSpinBox>

SpinBox::SpinBox(QString const &name_, int min_, int max_, int default_,
                 bool balance_, QWidget *parent)
    : QWidget(parent), ui(new Ui::SpinBox), balance(balance_) {
  setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  ui->setupUi(this);

  ui->name->setText(name_);

  if (min_ > max_) {
    qCritical("SpinBox min_ (%d) must be <= max_ (%d)", min_, max_);
    Q_ASSERT(min_ >= max_);
    return;
  }

  ui->spinBox->setMinimum(min_);
  ui->spinBox->setMaximum(max_);
  ui->spinBox->setAccelerated(true);
  ui->spinBox->setKeyboardTracking(false);

  connect(ui->spinBox, &QSpinBox::valueChanged, this, &SpinBox::valueChanged);

  ui->spinBox->setValue(default_);
}

SpinBox::~SpinBox() { delete ui; }

QString SpinBox::name() const { return ui->name->text(); }

int SpinBox::value() const { return ui->spinBox->value(); }

void SpinBox::setValue(int val_) {
  if (val_ == ui->spinBox->value()) {
    return;
  }

  ui->spinBox->setValue(val_);
}
