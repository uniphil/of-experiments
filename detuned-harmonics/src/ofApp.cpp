#include "ofApp.h"

double wave(uint64_t step, int sample_rate, double f) {
    double skew = 0;
    double phase_step = (f * 7) / sample_rate * TWO_PI;
    double phase_step_2 = (f * 11 + skew * 10) / sample_rate * TWO_PI;
    double phase_step_3 = (f * 13 + skew * 20) / sample_rate * TWO_PI;
    double phase_step_4 = (f * 19 + skew * 40) / sample_rate * TWO_PI;
    return sin(step * phase_step) +
           sin(step * phase_step_2) * 1/2 +
           sin(step * phase_step_3) * 1/4 +
           sin(step * phase_step_4) * 1/8;
}

double envelope(double age, double death, double attack, double sustain, double release) {
    double out = 1.0;
    if (age < 0) return 0;  // wat
    if (age < attack) {
        out = age / attack;
    } else {
        double t = age - attack;
        out = 1 / (pow(t,2) * pow(sustain * 10, 2) + 1);
    }
    if (death > 0) {
        if (age < death) {
            // wat (death is known ahead of time?)
        } else if (age - death > release) {
            // dead and expired
            out = 0;
        } else {
            out *= ofMap(age, death, death + release, 1, 0);
        }
    }
    return out;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(2, 0, 44100, 128, 2);
    step = 0;
    f = 440.0;
    note_start = 0.0;
    attack = 0.06;
    release = 0.5;
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    double frameTime = ofGetElapsedTimeMillis() / 1000.0;
    double death = 1.0;
    for (int i = 0; i < outBuffer.getNumFrames(); i++) {
        double age = frameTime + (i / (double)outBuffer.getSampleRate()) - note_start;
        double env = envelope(age, death, attack, 0.0, release);
        double sample = wave(step + i, outBuffer.getSampleRate(), f) * env * 0.4;
        outBuffer.getSample(i, 0) = sample;
        outBuffer.getSample(i, 1) = sample;
    }
    step += outBuffer.getNumFrames();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    note_start = ofGetElapsedTimeMillis() / 1000.0;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    f = ofMap(y, ofGetHeight(), 0, 27, 440);
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
