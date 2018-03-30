#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    source.setup(W, H);
    framed.allocate(N, N, OF_IMAGE_COLOR);

    window = new Hann(N);

    forward = kiss_fft_alloc(N, false, 0, 0);
    linearized = new kiss_fft_cpx[N];
    for (int i = 0; i < N; i++) linearized[i].i = 0;
    frequencies = new kiss_fft_cpx[N];
    
    theta = 0;
    
    ofSoundStreamSetup(2, 0, F, P, 2);

    sound = new Sound(69, 1.0, 0);

    ofSetBackgroundAuto(false);
    ofBackground(0);
    
    lastRepitch = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update(){
    theta = fmod(ofGetElapsedTimeMillis() / 1000.0 * TWO_PI / RP, TWO_PI);

    source.update();
    if (source.isFrameNew()) {
        ofPixels sourcePx = source.getPixels();
        for (int y = 0; y < N; y++) {
            int sy = y + source.getHeight() / 2 - N / 2;
            for (int x = 0; x < N; x++) {
                int sx = x + source.getWidth() / 2 - N / 2;
                framed.setColor(x, y, sourcePx.getColor(sx, sy));
            }
        }
        framed.mirror(false, true);
        framed.update();

        for (int i = 0; i < N; i++) {
            int x = ofMap(i, 0, N, -N / 2, N / 2) * cos(theta) + N / 2;
            int y = ofMap(i, 0, N, -N / 2, N / 2) * sin(theta) + N / 2;
            linearized[i].r = .9 * linearized[i].r +
                              .1 * framed.getColor(x, y).getLightness();
        }
        window->apply(linearized);

        kiss_fft(forward, linearized, frequencies);
    }
    
    if (ofGetElapsedTimef() > lastRepitch + 0.4) {
        sound->repitch(round(ofRandom(40, 70)));
        lastRepitch = ofGetElapsedTimef();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0xFF);
    framed.draw(0, 0);
    
    ofPushMatrix();
    ofTranslate(N / 2, N / 2);
    ofScale(N / 2, N / 2);
    ofDrawLine(-cos(theta), -sin(theta), cos(theta), sin(theta));
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(N, 0);
    for (int i = 0; i < N; i++) {
        ofSetColor(linearized[i].r);
        ofDrawCircle(i, ofGetFrameNum() % N, 1);
    }
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(N*2, 0);
    for (int i = 0; i < N / 2; i++) {
        ofSetColor(log(abs(frequencies[i])) * 10);
        ofDrawCircle(i, ofGetFrameNum() % N, 1);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    uint64_t t;
    for (size_t frame = 0; frame < outBuffer.getNumFrames(); frame++) {
        t = (audioFrame + frame) * 1000000 / F;
        float a = sound->getSample(t, frequencies, N);
        outBuffer.getSample(frame, 0) = a;
        outBuffer.getSample(frame, 1) = a;
    }
    audioFrame += outBuffer.getNumFrames();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
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
