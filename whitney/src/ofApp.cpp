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
    double vx = LAUNCH_VELOCITY * sin(theta);
    double vy = LAUNCH_VELOCITY * cos(theta);
    dashed(posToPx(ofPoint(0, 0)), posToPx(ofPoint(1000, 1000)), 8);
}

void drawPlane(double now, Plane plane) {
    if (plane.struck > 0 || plane.position(now) > pxToMetres(ofGetWidth() / 2)) return;
    ofSetColor(0xFF);
    ofDrawCircle(posToPx(ofPoint(plane.position(now), plane.altitude)), 10);
}

void drawInfo(double height, double aimTheta, double velocity) {
    ofSetColor(0x22, 0xFF, 0x44);
    stringstream reportStr;
    reportStr << std::fixed << std::setprecision(0) << std::setfill('0')
              << "target height: " << std::setw(5) << height << " m" << endl
              << " target speed: " << std::setw(5) << velocity << " m/s" << endl
              << "          aim: " << std::setw(4) << std::setprecision(2) << aimTheta <<  " rad";
    ofDrawBitmapString(reportStr.str(), 20, 20);
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
    aimTheta = viewTheta / 2;
    
    // measure velocity
    double viewPosition = aimHeight / tan(viewTheta);
    double velocity = (viewPosition - lastViewPosition) / dt;
    viewVelocity = (viewVelocity * 5 + velocity) / 6;  // some averaging to smooth it out

    // should a new plane appear?
    // weird actual probability https://eev.ee/blog/2018/01/02/random-with-care/#random-frequency
    double lastAppearance = planes.size() == 0 ? -100 : planes.back().appeared;
    double timeSince = (now / 1000000.0) - lastAppearance;
    double increasingWeight = (1.0 - 1.0 / timeSince) * BORINGNESS;
    if (ofRandom(1) < increasingWeight * dt) {
        Plane plane(now / 1000000.0, pxToPos(0, 0).x, ofRandom(500, VIEW_HEIGHT - 100), ofRandom(50, 300));
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
    drawInfo(aimHeight, aimTheta, viewVelocity);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
