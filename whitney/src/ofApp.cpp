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

void dashed(ofPoint a, ofPoint b, int period) {
    int steps = (b - a).length() / period;
    for (int i = 0; i < steps; i++) {
        double x1 = a.x + i * (b.x - a.x) / steps;
        double y1 = a.y + i * (b.y - a.y) / steps;
        double x2 = a.x + (i + 0.5) * (b.x - a.x) / steps;
        double y2 = a.y + (i + 0.5) * (b.y - a.y) / steps;
        ofDrawLine(x1, y1, x2, y2);
    }
}

void drawTarget(ofPoint pos) {
    ofPoint x(1, 0);
    ofPoint y(0, 1);
    
    ofSetColor(0xFF, 0xFF, 0xFF, 0x44);
    dashed(ofPoint(ofGetWidth() / 2, ofGetHeight()), posToPx(pxToPos(pos.x, pos.y) * 2), 8);
    dashed(ofPoint(0, pos.y), ofPoint(ofGetWidth(), pos.y), 8);

    int crossSize = 16;
    ofSetColor(0xFF, 0, 0);
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

//--------------------------------------------------------------
void ofApp::setup(){
    mouseAim.set(1, 1);
    aimHeight = 0;
    viewTheta = 0;
    aimTheta = 0;
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

    lastUpdate = now;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetBackgroundColor(0x44);
    drawTarget(posToPx(ofPoint(aimHeight / tan(viewTheta), aimHeight)));
    drawTrajectory(aimTheta);
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
