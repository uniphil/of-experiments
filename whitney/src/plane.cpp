#include "plane.hpp"

Plane::Plane(double appearance, double pos, double a, double v, bool big) {
    length = (big ? 23 : 10) * EASY; // metres
    appeared = appearance;
    struck = -1;
    initialPosition = pos;
    altitude = a;
    velocity = v;
}

double Plane::position(double now) {
    double dt = now - appeared;
    double moved = velocity * dt;
    return initialPosition + moved;
}

bool Plane::strike(double now, ofPoint p) {
    if (abs(p.y - altitude) > 15) return false;
    double x = position(now);
    if (abs(p.x - x) > length / 2) return false;
    struck = now;
    return true;
}
