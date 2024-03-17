#ifndef RENDERAREA_H
#define RENDERAREA_H

#include "simulation.h"

#include <QBrush>
#include <QPainterPath>
#include <QPen>
#include <QResizeEvent>
#include <QWidget>

#include <vector>

/// \brief This widget is responsible for visualizing all boids
class RenderArea : public QWidget {
  Q_OBJECT
public:
  explicit RenderArea(QWidget *parent = nullptr);

  QSize minimumSizeHint() const override;
  QSize sizeHint() const override;

  void setBoids(std::vector<boid> const &boids_) {
    boids = boids_;
    update();
  }

signals:
  void resized(QResizeEvent *event_);

public slots:
  /// \brief Handle config value changed
  void handleValueChanged(QString const &name_, int val_);

  void handleDrawNeighborhood(bool draw_);

protected:
  void paintEvent(QPaintEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void paintBoid(QPainter &painter_, boid const &boid_) const;

private:
  QPen pen;
  QBrush brush;
  QPainterPath shape;
  int neighborhood;
  bool drawNeighborhood;

  std::vector<boid> boids;
};

#endif // RENDERAREA_H
