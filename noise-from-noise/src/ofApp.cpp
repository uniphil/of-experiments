#include "ofApp.h"

void scale(kiss_fft_cpx & bin, double amount) {
    bin.r *= amount;
    bin.i *= amount;
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofSoundStreamSetup(C, 0, R, N / 2, 2);

    kissForward = kiss_fft_alloc(N, false, 0, 0);
    kissInverse = kiss_fft_alloc(N, true, 0, 0);
    lastTimeSamples = new kiss_fft_cpx[N * C];
    timeSamples = new kiss_fft_cpx[N];
    for (int i = 0; i < N; i++) timeSamples[i].i = 0;
    frequencyBins = new kiss_fft_cpx[N];
    
    ofBackground(0);
    ofSetBackgroundAuto(false);
    ofHideCursor();
    
    max = -INFINITY;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    int h = ofGetHeight();
    int w = ofGetWidth();

    ofSetColor(0xCC);
    ofDrawLine(mouseX, 0, mouseX, h);
    ofDrawLine(0, mouseY, w, mouseY);

    ofSetColor(0, 0, 0, 0x88);
    for (int i = 0; i < 100; i++) {
        if (i % 2) {
            ofDrawLine(ofRandom(w), 0, ofRandom(w), h);
        } else {
            ofDrawLine(0, ofRandom(h), w, ofRandom(h));
        }
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    float highestBin = HIGHEST * N / R;
    float bin = ofMap(pow(ofMap(mouseX, 0, ofGetWidth(), 0, 1, true), 2),
                      0, 1, 0, highestBin);
    float filter = pow(ofMap(mouseY, 0, ofGetHeight(), 0, 1, true), 0.25);
    float base = 1.0 / (N + 2) + filter / N;

    for (int channel = 0; channel < outBuffer.getNumChannels(); channel++) {
        for (int i = 0; i < N; i++) {
            timeSamples[i].r = ofRandom(-1, 1);
            timeSamples[i].i = 0;
        }
        kiss_fft(kissForward, timeSamples, frequencyBins);
        for (int i = 0; i < N / 2; i++) {
            float amount = 1 - filter;
            if (i == (int)(bin * 1)) amount = 1;
            if (i == (int)(bin * 2)) amount += (1 - amount) / 8;
            if (i == (int)(bin * 3)) amount = -(amount + (1 - amount) / 9);
            if (i == (int)(bin * 4)) amount += (1 - amount) / 3;
            if (i == (int)(bin * 5)) amount += (1 - amount) / 6;
            if (i == (int)(bin * 6)) amount = -(amount + (1 - amount) / 8);
            if (i == (int)(bin * 7)) amount += (1 - amount) / 6;
            if (i == (int)(bin * 8)) amount += (1 - amount) / 6;
            scale(frequencyBins[i], amount);
            scale(frequencyBins[N - i], amount);
        }
        kiss_fft(kissInverse, frequencyBins, timeSamples);
        for (int i = 0; i < outBuffer.getNumFrames(); i++) {
            float sample = timeSamples[i].r;
            if (i < N / 2) {
                float last = lastTimeSamples[N * channel + i + N / 2].r;
                float fade = ofMap(i, 0, N / 2, 0, 1);
                sample *= sqrt((1 + fade) / 2);
                sample += sqrt((1 - fade) / 2) * last;
            }
            sample *= base;
            outBuffer.getSample(i, channel) = sample;
            if (abs(sample) > max) {
                max = abs(sample);
                cout << "new max: " << max << endl;
            }
        }
        for (int i = 0; i < N; i++) {
            lastTimeSamples[N * channel + i] = timeSamples[i];
        }
    }
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
