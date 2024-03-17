#include "simulation.h"
#include "keys.h"

#include <chrono>
using namespace std::chrono;

#include <algorithm>
#include <cmath>
using namespace std;

namespace {
double constexpr M_2PI = 2 * M_PI;

double constexpr MAX_TURN_RATE = M_2PI / 4.0; // rad/s
double constexpr MAX_SPEED = 100;             // px/s

double constexpr TURN_RADIUS = MAX_SPEED * MAX_TURN_RATE;

double constexpr MAX_BOUND = 500;

struct vector_t {
  double x;
  double y;
};

struct rule_t {
  vector_t vector;
  int weight;
};

double constrainAngle(double x_) {
  x_ = fmod(x_, M_2PI);

  if (x_ < 0)
    x_ += M_2PI;

  return x_;
}

double distance(boid const &a_, boid const &b_) {
  return hypot(a_.x - b_.x, a_.y - b_.y);
}

double time() {
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch())
             .count() /
         1000.;
}
} // namespace

simulation::simulation(QObject *parent)
    : QObject{parent}, physicsTimer(), boids(), lastTick(time()), curBoid(0),
      m_w(MAX_BOUND), m_h(MAX_BOUND), neighborhood(0), weights() {
  connect(&physicsTimer, &QTimer::timeout, this, &simulation::step);
}

void simulation::start() {
  physicsTimer.start(10);
  process();
}

void simulation::handleResized(QResizeEvent *event_) {
  m_w = event_->size().width();
  m_h = event_->size().height();
}

void simulation::handleValueChanged(QString const &name_, int val_) {
  if (RULES.count(name_)) {
    weights[name_.toStdString()] = val_;
    return;
  }

  if (name_ == BOIDS) {
    handleNumBoidsChanged(val_);
  } else if (name_ == NEIGHBORHOOD) {
    neighborhood = val_;
  }
}

void simulation::step() {
  double const tick = time();
  double const dt = tick - lastTick;
  lastTick = tick;

  for (auto &b : boids) {
    b.heading = constrainAngle(b.heading + b.turnRate * dt);

    double const dist = MAX_SPEED * dt;
    b.x += dist * cos(b.heading);
    b.y += dist * sin(b.heading);
  }

  emit update(boids);
}

void simulation::handleNumBoidsChanged(int val_) {
  size_t const prevSize = boids.size();
  boids.resize(val_);

  curBoid = (val_ > 0) ? curBoid % val_ : 0;

  if (val_ <= prevSize) {
    return;
  }

  double const w = m_w / 2.;
  double const h = m_h / 2.;
  for (size_t i = prevSize; i < boids.size(); ++i) {
    boids[i].x = float(rand() % m_w) - w;
    boids[i].y = float(rand() % m_h) - h;
    boids[i].heading = constrainAngle(rand());
    boids[i].turnRate = constrainAngle(rand()) / 4.;
  }
}

void simulation::process() {
  QTimer::singleShot(0, this, &simulation::process);

  if (!boids.size()) {
    return;
  }

  // Current boid to process rules for should always
  // be in bounds, but catching this to be safe
  curBoid = curBoid % boids.size();

  auto &boid = boids[curBoid];

  double avgX = 0.;
  double avgY = 0.;

  double avgVX = 0.;
  double avgVY = 0.;

  double avgNearX = 0.;
  double avgNearY = 0.;
  double const nearRange = neighborhood / 3.;

  for (size_t i = 0; i < boids.size(); ++i) {
    if (i == curBoid) {
      continue;
    }

    auto const &n = boids[i];

    double const dist = distance(boid, n);
    if (dist > neighborhood) {
      continue;
    }

    avgX += (n.x - boid.x);
    avgY += (n.y - boid.y);

    // Compute average heading of neighborhood
    avgVX += cos(n.heading);
    avgVY += sin(n.heading);

    // Compute average separation of nearby neighbors
    // Influence fades as neighbors get farther away
    // dx and dy are computed inversely to push neighbors apart
    double const dx = (boid.x - n.x);
    double const dy = (boid.y - n.y);
    avgNearX += (dx != 0.) ? nearRange / dx : 0.;
    avgNearY += (dy != 0.) ? nearRange / dy : 0.;
  }

  curBoid = ++curBoid % boids.size();

  auto const wallBounce = [](double val_, double bound_) -> double {
    if (bound_ - abs(val_) > 1.5 * TURN_RADIUS) {
      return 0.;
    }

    return -val_;
  };

  double wallX = wallBounce(boid.x, m_w / 2.);
  double wallY = wallBounce(boid.y, m_h / 2.);

  // Convert to unit vectors
  auto const normalize = [](double &x_, double &y_) {
    double const mag = sqrt(x_ * x_ + y_ * y_);
    x_ = (mag == 0.) ? 0. : x_ / mag;
    y_ = (mag == 0.) ? 0. : y_ / mag;
  };

  normalize(avgX, avgY);
  normalize(avgVX, avgVY);
  normalize(avgNearX, avgNearY);
  normalize(wallX, wallY);

  // Weighted sum of the following rules:
  // 1. Match average velocities of local neighborhood
  // 2. Move away from boundaries
  // 3. Move towards center of local neighborhood
  // 4. Avoid going too near flock members
  vector<rule_t> rules;

  rule_t alignment;
  alignment.weight = weights["Alignment"];
  alignment.vector.x = avgVX;
  alignment.vector.y = avgVY;

  rule_t bounds;
  bounds.weight = weights["Bounds"];
  bounds.vector.x = wallX;
  bounds.vector.y = wallY;

  rule_t cohesion;
  cohesion.weight = weights["Cohesion"];
  cohesion.vector.x = avgX;
  cohesion.vector.y = avgY;

  rule_t separation;
  separation.weight = weights["Separation"];
  separation.vector.x = avgNearX;
  separation.vector.y = avgNearY;

  rules.push_back(alignment);
  rules.push_back(bounds);
  rules.push_back(cohesion);
  rules.push_back(separation);

  double finalVX = 0.;
  double finalVY = 0.;
  for (auto const &rule : rules) {
    finalVX += rule.weight * rule.vector.x;
    finalVY += rule.weight * rule.vector.y;
  }

  normalize(finalVX, finalVY);
  double const finalMag = sqrt(finalVX * finalVX + finalVY * finalVY);
  if (finalMag == 0.) {
    boid.turnRate = 0;
    return;
  }

  double const heading = constrainAngle(atan2(finalVY, finalVX));

  double relheading = heading - boid.heading;
  if (relheading > M_PI)
    relheading -= M_2PI;
  else if (relheading < -M_PI)
    relheading += M_2PI;

  boid.turnRate = clamp(relheading, -MAX_TURN_RATE, MAX_TURN_RATE);
}
