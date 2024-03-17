#ifndef SPINBOX_H
#define SPINBOX_H

#include <QString>
#include <QWidget>

namespace Ui {
class SpinBox;
}

/// \brief The SpinBox is a helper class to manage balancing all of the config
/// values to sum to 100. They can opt in to balancing by setting the balance_
/// parameter to true.
class SpinBox : public QWidget {
  Q_OBJECT

public:
  /// \brief SpinBox constructor
  /// \param name_ The name of the SpinBox
  /// \param min_ The minimum SpinBox value
  /// \param max_ The maxmium SpinBox value
  /// \param default_ The default SpinBox value
  /// \param balance_ Whether or not this SpinBox's value should be balanced
  /// with others
  /// \param parent parent widget
  explicit SpinBox(QString const &name_, int min_, int max_, int default_,
                   bool balance_ = false, QWidget *parent = nullptr);
  ~SpinBox();

  QString name() const;

  bool shouldBalance() const { return balance; }

  int value() const;

  /// \brief Set the value of the SpinBox
  /// \param val_ The value to set
  void setValue(int val_);

signals:
  /// \brief Signal emitted when the value of the SpinBox has changed
  /// \param val_ The new value
  void valueChanged(int val_);

private:
  Ui::SpinBox *ui;
  bool balance;
};

#endif // SPINBOX_H
