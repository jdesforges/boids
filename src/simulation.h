#ifndef SIMULATION_H
#define SIMULATION_H

#include <QObject>
#include <QResizeEvent>
#include <QString>
#include <QTimer>

#include <string>
#include <unordered_map>
#include <vector>

/// \brief A single boid containing its identifier and state information.
struct boid {
  size_t id;
  float x;
  float y;
  float heading;
  float turnRate;
};

/// \brief This object is responsible for simulating the boids. The simulation
/// utilizes a timer operating on a 100Hz cycle. On each cycle, every boid has
/// its state updated according to the time elapsed since the last update. Then
/// the boids rules are processed for a single boid, which operates in a round
/// robin fashion to ensure all boids get equal opportunities for processing
/// their rules without causing any delays in publishing the state updates for
/// visualization.
/// The position of bounds are "unbounded" (precision determined by the system's
/// representation of doubles), however, the boids are encouraged to remain
/// within the visual boundary. There is a weight associated with this rule, so
/// it can be disabled entirely if desired. The boundary does resize with the
/// application's size.
///
/// The boids process all of their rules using a configurable neighborhood
/// distance, which is used to determine what surrounding boids are in the
/// current boid's local neighborhood. If a boid is not within the neighborhood
/// distance, then it is ignored for rules processing.
///
/// There are three rules that boids adhere to along with the boundary rule:
/// 1. Cohesion - The boids prefer to maintain close proximity to their
/// neighborhood's average position.
/// 2. Alignment - The boids prefer to maintain a heading that is aligned with
/// the average heading of their local neighborhood.
/// 3. Separation - The boirds prefer to maintain a minimum distance from each
/// of their local neighbors. If two boids begins are too close to each other,
/// then the boids will "push" their headings away from each other to maintain
/// separation.
///
/// \note The boids only have control over their heading and move around at a
/// fixed speed. Because of this, the separation rule can typically result in
/// the boids flying in a relatively straight line that sweeps across an area.
/// This is somewhat similar to birds flying in a V formation, however, that is
/// entirely coincidental and the behavior could be improved by giving the boids
/// control over their speed.
///
/// \note There is nothing that prevents boids from "crashing" into one another,
/// and nothing significant happens when that occurs.
class simulation : public QObject {
  Q_OBJECT
public:
  explicit simulation(QObject *parent = nullptr);

  /// \brief Start the boids simulation
  void start();

signals:
  /// \brief Signal emitted when there is a new boids update to visualize
  void update(std::vector<boid> const &boids_);

public slots:
  /// \brief Handle resizing
  void handleResized(QResizeEvent *event_);

  /// \brief Handle config value changes
  void handleValueChanged(QString const &name_, int val_);

private slots:
  /// \brief Physics step
  void step();

private:
  /// \brief Handle number of boids changed
  void handleNumBoidsChanged(int val_);

  /// \brief Boid rules step
  void process();

  QTimer physicsTimer;
  std::vector<boid> boids;
  double lastTick;
  size_t curBoid;
  int m_w;
  int m_h;
  double neighborhood;
  std::unordered_map<std::string, int> weights;
};

#endif // SIMULATION_H
