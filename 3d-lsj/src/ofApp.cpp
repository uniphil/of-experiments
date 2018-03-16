#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    double t = ofGetElapsedTimeMicros() / 1000000.0 / 2.0;
    ofBackground(0);
    cam.begin();
    for (int x = 0; x < N; x++) {
        ofPushMatrix();
        ofTranslate(x * SCALE - N * SCALE / 2, 0);
        for (int y = 0; y < N; y++) {
            ofPushMatrix();
            ofTranslate(0, y * SCALE - N * SCALE / 2);
            for (int z = 0; z < N; z++) {
                ofPushMatrix();
                ofTranslate(ofPoint(0, 0, z * SCALE - N * SCALE / 2));

                ofPoint p;
                ofPolyline line;

                for (int i = 0; i < 300; i++) {
                    double tn = t - i * 0.016;
                    ofPoint linePoint;
                    linePoint.x = ofMap(sin(tn * (1 + x * SHIFT_STEP)), -1, 1, 0, SCALE);
                    linePoint.y = ofMap(cos(tn * (1 + y * SHIFT_STEP)), -1, 1, 0, SCALE);
                    linePoint.z = ofMap(sin(tn * (1 + z * SHIFT_STEP)), -1, 1, 0, SCALE);
                    line.addVertex(linePoint);
                    if (i == 0) {
                        p = linePoint;
                    }
                }

                ofSetColor(0xFF, 0xFF, 0xFF, 0x22);
                line.draw();

                ofSetColor(0xFF, 0xFF, 0xFF, 0x44);
                ofDrawCircle(p, 0.5);

                ofPopMatrix();
            }
            ofPopMatrix();
        }
        ofPopMatrix();
    }
    cam.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
