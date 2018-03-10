#include "ofApp.h"

double pxToMetres(int px) {
    double scale = (double)VIEW_HEIGHT / ofGetHeight();
    return px * scale;
}

ofPoint pxToPos(int xPos, int yPos) {
    double x = pxToMetres(xPos - ofGetWidth() / 2);
    double y = pxToMetres(ofGetHeight() - yPos);
    return ofPoint(x, y);
}

double metresToPx(double metres) {
    double scale = (double)ofGetHeight() / VIEW_HEIGHT;
    return metres * scale;
}

ofPoint posToPx(ofPoint pos) {
    double x = ofGetWidth() / 2 + metresToPx(pos.x);
    double y = ofGetHeight() - metresToPx(pos.y);
    return ofPoint(x, y);
}

void dashed(ofPoint a, ofPoint b, int period, float duty) {
    int steps = (b - a).length() / period;
    for (int i = 0; i < steps; i++) {
        double x1 = a.x + i * (b.x - a.x) / steps;
        double y1 = a.y + i * (b.y - a.y) / steps;
        double x2 = a.x + (i + duty) * (b.x - a.x) / steps;
        double y2 = a.y + (i + duty) * (b.y - a.y) / steps;
        ofDrawLine(x1, y1, x2, y2);
    }
}
void dashed(ofPoint a, ofPoint b, int period) {
    dashed(a, b, period, 0.5);
}

void drawTarget(ofPoint pos) {
    ofPoint x(1, 0);
    ofPoint y(0, 1);
    
    ofSetColor(0xFF, 0xFF, 0xFF, 0x44);
    double scaleToScreen = (double)ofGetHeight() / (ofGetHeight() - pos.y);
    dashed(ofPoint(ofGetWidth() / 2, ofGetHeight()), posToPx(pxToPos(pos.x, pos.y) * scaleToScreen), 8, 0.25);
    dashed(ofPoint(0, pos.y), ofPoint(ofGetWidth(), pos.y), 8, 0.25);

    int crossSize = 16;
    ofSetColor(0xFF, 0x22, 0);
    ofSetLineWidth(2);
    ofDrawLine(pos + (-y * crossSize), pos + (y * crossSize));
    ofDrawLine(pos + (-x * crossSize), pos + (x * crossSize));
    ofSetLineWidth(1);
    ofNoFill();
    ofDrawCircle(pos, crossSize - 5);
}

void drawTrajectory(double theta) {
    ofSetColor(0xFF, 0xFF, 0, 0x44);
    for (double t = 0; t < 10; t += 0.0667) {
        double x = LAUNCH_VELOCITY * t * cos(theta);
        double y = LAUNCH_VELOCITY * t * sin(theta) - (0.5 * GRAVITY * pow(t, 2));
        ofFill();
        ofDrawCircle(posToPx(ofPoint(x, y)), 1);
    }
}

void drawPlane(double now, Plane plane) {
    if (plane.struck > 0 || plane.position(now) > pxToMetres(ofGetWidth() / 2)) return;
    ofSetColor(0xFF);
    ofNoFill();
    ofDrawCircle(posToPx(ofPoint(plane.position(now), plane.altitude)), 10);
}

void drawProjectile(double now, Projectile projectile) {
    if (projectile.struck > 0) return;
    ofSetColor(0xFF, 0xCC, 0x00);
    ofDrawCircle(posToPx(projectile.position(now)), 3);
}

void drawInfo(double height, double aimTheta, double velocity, bool ready) {
    ofSetColor(0x22, 0xFF, 0x44);
    stringstream reportStr;
    reportStr << std::fixed << std::setprecision(0) << std::setfill('0')
              << "target height: " << std::setw(5) << height << " m" << endl
              << " target speed: " << std::setw(5) << velocity << " m/s" << endl
              << "          aim: " << std::setw(4) << std::setprecision(2) << aimTheta <<  " rad" << endl
              << "        canon: " << (ready ? "ready" : "reloading");
    ofDrawBitmapString(reportStr.str(), 20, 20);
}

bool canonReady(double now, vector <Projectile> projectiles) {
    double lastFire = projectiles.size() == 0 ? 0 : projectiles.back().appeared;
    double timeSince = now - lastFire;
    return timeSince > RELOAD_TIME;
}

double timeOfImpact(double x, double y, double planeVelocity, double projectileVelocity) {
    double a = pow(projectileVelocity, 2) - pow(planeVelocity, 2);
    double b = x * planeVelocity;
    double c = pow(x, 2) + pow(y, 2);
    double d = pow(b, 2) + a * c;
    return (b + sqrt(d)) / a;
//    double t = 0;
//    if (d >= 0) {
//        t = (b + sqrt(d)) / a;
//        if (t < 0) {
//            t = 0;
//        }
//    }
//    return t;
}

//--------------------------------------------------------------
void ofApp::setup(){
    lastViewPosition = 0;
    mouseAim.set(1, 1);
    aimHeight = VIEW_HEIGHT / 2;
    viewTheta = PI;
    aimTheta = PI;
}

//--------------------------------------------------------------
void ofApp::update(){
    uint64_t now = ofGetElapsedTimeMicros();  // useconds
    double dt = (now - lastUpdate) / 1000000.0;  // seconds

    // for now, height just updates live
    aimHeight = mouseAim.y;

    // changing the angle we're pointing is constrained to a max rotational speed
    double targetTheta = mouseAim.x > 0
        ? atan(mouseAim.y / mouseAim.x)
        : PI - atan(mouseAim.y / abs(mouseAim.x));
    double deltaTheta = targetTheta - viewTheta;
    double maxThetaStep = (double)MAX_SCOPE_TRACK * dt;
    if (abs(deltaTheta) > maxThetaStep) {
        deltaTheta = deltaTheta > 0 ? maxThetaStep : -maxThetaStep;
    }
    viewTheta += deltaTheta;

    // measure velocity
    double viewPosition = aimHeight / tan(viewTheta);
    double velocity = (viewPosition - lastViewPosition) / dt;
    viewVelocity = (viewVelocity * 99 + velocity) / 100;  // some averaging to smooth it out

    // aim the cannon
    // the projectile moves pretty quick, so we ignore gravity
    // ...it's a cheat. including gravity requires solving a quartic :/
    double leadTime = timeOfImpact(viewPosition, aimHeight, viewVelocity, LAUNCH_VELOCITY);
    double impactPosition = viewPosition + leadTime * viewVelocity;
    aimTheta = impactPosition > 0
        ? atan(aimHeight / impactPosition)
        : PI - atan(aimHeight / abs(impactPosition));

    // should a new plane appear?
    // weird actual probability https://eev.ee/blog/2018/01/02/random-with-care/#random-frequency
    double lastAppearance = planes.size() == 0 ? -100 : planes.back().appeared;
    double timeSince = (now / 1000000.0) - lastAppearance;
    double increasingWeight = (1.0 - 1.0 / timeSince) * BORINGNESS;
    if (ofRandom(1) < increasingWeight * dt) {
        double vel = sqrt(ofRandom(1)) / 2;  // boo random sucks I just want a nice symmetric hump-looking beta dist :(
        if (ofRandom(1) > 0.5) {  // the other side of the hump thing
            vel = 1 - vel;
        }
        vel = vel * (PLANE_MAX_SPEED - PLANE_MIN_SPEED) * 2 + PLANE_MIN_SPEED;
        Plane plane(now / 1000000.0, pxToPos(0, 0).x, ofRandom(800, VIEW_HEIGHT - 100), ofRandom(50, 300));
        planes.push_back(plane);
    }

    lastUpdate = now;
    lastViewPosition = viewPosition;
}

//--------------------------------------------------------------
void ofApp::draw(){
    double now = ofGetElapsedTimeMicros()/ 1000000.0;  // seconds
    ofSetBackgroundColor(0x22);
    drawTarget(posToPx(ofPoint(aimHeight / tan(viewTheta), aimHeight)));
    drawTrajectory(aimTheta);
    for (int i = 0; i < planes.size(); i++) {
        drawPlane(now, planes[i]);
    }
    for (int i = 0; i < projectiles.size(); i++) {
        drawProjectile(now, projectiles[i]);
    }
    drawInfo(aimHeight, aimTheta, viewVelocity, canonReady(now, projectiles));
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key != ' ') return;
    double now = ofGetElapsedTimeMicros() / 1000000.0;
    if (!canonReady(now, projectiles)) return;
    Projectile projectile(now, aimTheta, LAUNCH_VELOCITY);
    projectiles.push_back(projectile);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    mouseAim = pxToPos(x, y <= 0 ? 1 : y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
