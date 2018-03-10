#pragma once
#include "ofMain.h"

class Projectile {
public:
    Projectile(double appearance, double launchAngle, double launchVelocity);

    ofPoint position(double now);

    double appeared;
    double struck;
    double launchAngle;
    double launchVelocity;
};
