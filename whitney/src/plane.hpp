#pragma once
#include "ofMain.h"

class Plane {
public:
    Plane(double appearance, double pos, double a, double v);
    
    double position(double now);
    bool strike(double now, ofPoint p);

    double length;
    double appeared;
    double initialPosition;
    double struck;
    double altitude;
    double velocity;
};
