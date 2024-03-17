#include "renderarea.h"
#include "keys.h"

#include <QPainter>
#include <QPoint>
#include <QSizePolicy>

RenderArea::RenderArea(QWidget *parent)
    : QWidget{parent}, pen(), brush(), shape(), neighborhood(0),
      drawNeighborhood(false), boids() {
  setBackgroundRole(QPalette::Base);

  pen.setWidth(1);

  brush.setStyle(Qt::SolidPattern);
  pen.setJoinStyle(Qt::RoundJoin);

  // Draw triangle for boids
  shape.moveTo(-3, 3);
  shape.lineTo(6, 0);
  shape.lineTo(-3, -3);
  shape.closeSubpath();

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize RenderArea::minimumSizeHint() const { return QSize(200, 200); }

QSize RenderArea::sizeHint() const { return QSize(400, 400); }

void RenderArea::handleValueChanged(QString const &name_, int val_) {
  if (name_ == NEIGHBORHOOD) {
    neighborhood = val_;
  }
}

void RenderArea::handleDrawNeighborhood(bool draw_) {
  drawNeighborhood = draw_;
}

void RenderArea::paintEvent(QPaintEvent * /* event */) {
  QPainter painter(this);
  painter.setPen(pen);
  painter.setBrush(brush);
  painter.translate(width() / 2, height() / 2);
  painter.setRenderHint(QPainter::Antialiasing, true);

  for (auto const &boid : boids) {
    paintBoid(painter, boid);
  }
}

void RenderArea::resizeEvent(QResizeEvent *event) { emit resized(event); }

void RenderArea::paintBoid(QPainter &painter_, boid const &boid_) const {
  painter_.save();

  painter_.translate(boid_.x, boid_.y);

  painter_.rotate(qRadiansToDegrees(boid_.heading));
  painter_.drawPath(shape);

  if (drawNeighborhood) {
    painter_.setBrush(QBrush());
    painter_.drawEllipse(-neighborhood, -neighborhood, 2 * neighborhood,
                         2 * neighborhood);
  }

  painter_.restore();
}
