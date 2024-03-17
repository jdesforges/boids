Boids Simulation Using Qt
=========================

Overview
--------

This project is a quick demonstration of a configurable boids simulation using Qt. The following details of the simulation are configurable:

1. Number of boids
2. The boid neighborhood distance
3. Weights for the following rules
   1. Bounds - Keeps the boids within view
   2. Alignment - Boids prefer to maintain a heading aligned with the neighborhood's average heading
   3. Cohesion - Boids prefer to be near their local neighborhood's center
   4. Separation - Boids should prevent collisions with each other by maintaining a minimum separation distance

Note that the sum of the weights can not be greater than 100, otherwise, the weights will be automatically adjusted.

If any weight is set to 0, that rule is effectively disabled.

The boids are not restricted from going off the screen, which is why the "Bounds" rule is put in place.

System Requirements
-------------------

* C++ compiler supporting the C++17 standard
* CMake
* Ninja
* Qt 6

Building
--------

Open a terminal and run the following in the top-level boids directory

```
mkdir build
cd build
cmake ../src
cmake --build .
```

Then run the "boids" application to start the simulation.
