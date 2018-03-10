//
//  projectile.cpp
//  whitney
//
//  Created by phil on 2018-03-10.
//

#include "projectile.hpp"
#define GRAVITY 9.81  // TODO share this w/ ofApp.h

Projectile::Projectile(double appearance, double angle, double vel) {
    struck = -1;
    appeared = appearance;
    launchAngle = angle;
    launchVelocity = vel;
}

ofPoint Projectile::position(double now) {
    double dt = now - appeared;
    double x = launchVelocity * dt * cos(launchAngle);
    double y = launchVelocity * dt * sin(launchAngle) - (0.5 * GRAVITY * pow(dt, 2));
    return ofPoint(x, y);
}
