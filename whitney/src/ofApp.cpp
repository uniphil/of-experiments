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
    ofSetColor(0xFF, 0x22, 0, 0x88);
    ofSetLineWidth(1);
    ofDrawLine(pos + (-y * crossSize), pos + (-y * crossSize / 3));
    ofDrawLine(pos + (y * crossSize), pos + (y * crossSize / 3));
    ofDrawLine(pos + (-x * crossSize), pos + (-x * crossSize / 3));
    ofDrawLine(pos + (x * crossSize), pos + (x * crossSize / 3));
    ofSetLineWidth(1);
    ofNoFill();
    ofDrawCircle(pos, crossSize - 5);
}

void drawTrajectory(double theta) {
    ofSetColor(0xFF, 0x22, 0, 0x44);
    for (double t = 0; t < 10; t += 0.016) {
        double x = LAUNCH_VELOCITY * t * cos(theta);
        double y = LAUNCH_VELOCITY * t * sin(theta) - (0.5 * GRAVITY * pow(t, 2));
        ofFill();
        ofDrawCircle(posToPx(ofPoint(x, y)), 1);
    }
}

void drawPlane(double now, Plane plane) {
    if (plane.struck > 0 || plane.position(now) > pxToMetres(ofGetWidth() / 2)) return;
    ofPoint pos(plane.position(now), plane.altitude);
    double l = plane.length / 2;
    ofPoint x(1, 0);
    ofPoint y(0, 1);
    ofPolyline outline;
    outline.addVertex(posToPx(pos + -1*l*x + -.3*l*y));  // bottom left
    outline.addVertex(posToPx(pos + 1*l*x + -.3*l*y));  // bottom right
    outline.addVertex(posToPx(pos + 1*l*x + -0.1*l*y));  // nose
    outline.addVertex(posToPx(pos + 0.8*l*x + 0*l*y));  // top of nose
    outline.addVertex(posToPx(pos + -0.7*l*x + 0*l*y));  // root of tail
    outline.addVertex(posToPx(pos + -0.8*l*x + 0.3*l*y));  // top of tail
    outline.addVertex(posToPx(pos + -0.95*l*x + 0.3*l*y));  // back top
    outline.addVertex(posToPx(pos + -1*l*x + -0.2*l*y));  // close it up
    ofSetColor(0x44, 0x88, 0xFF);
    ofNoFill();
    outline.draw();
}

void drawProjectile(double now, Projectile projectile) {
    if (projectile.struck > 0) return;
    ofFill();
    ofSetColor(0xFF, 0x88, 0x00);
    ofDrawCircle(posToPx(projectile.position(now)), 1.5);
}

void drawInfo(double height, double aimTheta, double velocity, bool ready) {
    ofSetColor(0x22, 0xFF, 0x44);
    stringstream reportStr;
    reportStr << std::fixed << std::setprecision(0) << std::setfill('0')
              << "target altitude: " << std::setw(5) << height << " m" << endl
              << "target velocity: " << std::setw(5) << velocity << " m/s" << endl
              << "bofors 40 state: " << (ready ? "ready. <space> to fire" : "reloading...") << endl
              << "      canon aim: " << std::setw(4) << std::setprecision(2) << aimTheta <<  " rad" << endl;
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
}

//--------------------------------------------------------------
void ofApp::setup(){
    lastViewPosition = 0;
    mouseAim.set(1, 1);
    aimHeight = VIEW_HEIGHT / 2;
    viewTheta = PI;
    aimTheta = PI;
    ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update(){
    uint64_t nowMicro = ofGetElapsedTimeMicros();  // useconds
    double now = nowMicro / 1000000.0;  // seconds
    double dt = (nowMicro - lastUpdate) / 1000000.0;  // seconds

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
    if (velocity < 0) velocity = 0;
    if (velocity > PLANE_MAX_SPEED * 2) velocity = PLANE_MAX_SPEED * 2;
    viewVelocity = (viewVelocity * 99 + velocity) / 100;  // some averaging to smooth it out

    // aim the cannon
    // the projectile moves pretty quick, so we ignore gravity
    // ...it's a cheat. including gravity requires solving a quartic :/
    double leadTime = timeOfImpact(viewPosition, aimHeight, viewVelocity, LAUNCH_VELOCITY);
    // fudge a gravity adjustment
    double lostHeight = 0.5 * GRAVITY * pow(leadTime, 2);
    double impactPosition = viewPosition + leadTime * viewVelocity;
    aimTheta = impactPosition > 0
        ? atan((aimHeight + lostHeight) / impactPosition)
        : PI - atan((aimHeight + lostHeight) / abs(impactPosition));

    // should a new plane appear?
    // weird actual probability https://eev.ee/blog/2018/01/02/random-with-care/#random-frequency
    double lastAppearance = planes.size() == 0 ? -100 : planes.back().appeared;
    double timeSince = now - lastAppearance;
    double increasingWeight = 1.0 - 1.0 / (1 + timeSince * BORINGNESS);
    cout << increasingWeight << endl;
    if (ofRandom(1) < increasingWeight * dt) {
        double vel = sqrt(ofRandom(1)) / 2;  // boo random sucks I just want a nice symmetric hump-looking beta dist :(
        if (ofRandom(1) > 0.5) {  // the other side of the hump thing
            vel = 1 - vel;
        }
        vel = vel * (PLANE_MAX_SPEED - PLANE_MIN_SPEED) * 2 + PLANE_MIN_SPEED;
        double alt = ofRandom(800, VIEW_HEIGHT - 100);
        double big = ofRandom(800, VIEW_HEIGHT - 100) < alt;
        Plane plane(now, pxToPos(0, 0).x, alt, vel, ofRandom(1) > 0.333);
        planes.push_back(plane);
    }
    
    // are there any hits?
    for (int i = 0; i < projectiles.size(); i++) {
        Projectile& proj = projectiles[i];
        if (proj.struck > 0) continue;

        for (int j = 0; j < planes.size(); j++) {
            Plane& plane = planes[j];
            if (plane.struck > 0) continue;
            if (plane.strike(now, proj.position(now))) {
                proj.struck = now;
                break;
            }
        }
    }

    lastUpdate = nowMicro;
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
