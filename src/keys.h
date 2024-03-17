#ifndef KEYS_H
#define KEYS_H

#endif // KEYS_H

#include <QString>

#include <set>

QString const ALIGNMENT = "Alignment";
QString const BOUNDS = "Bounds";
QString const COHESION = "Cohesion";
QString const SEPARATION = "Separation";

std::set<QString> const RULES = {ALIGNMENT, BOUNDS, COHESION, SEPARATION};

QString const BOIDS = "Boids";

QString const NEIGHBORHOOD = "Neighborhood";
