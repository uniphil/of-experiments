#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    source.setup(W, H);
    framed.allocate(N, N, OF_IMAGE_COLOR);

    window = new Hann(N);

    forward = kiss_fft_alloc(N, false, 0, 0);
    linearized = new kiss_fft_cpx[N];
    for (int i = 0; i < N; i++) linearized[i].r = linearized[i].i = 0;
    frequencies = new kiss_fft_cpx[N];
    for (int i = 0; i < N; i++) frequencies[i].r = frequencies[i].i = 0;
    
    theta = 0;
    
    ofSoundStreamSetup(2, 0, F, P, 2);

    midiIn.openPort(0);
    midiIn.addListener(this);

    ofSetBackgroundAuto(false);
    ofBackground(0);
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
    
    vector <unsigned int> pitchesToRemove;
    for (int i = 0; i < notes.size(); i++) {
        Sound note = notes[i];
        if (note.dead(audioFrame)) pitchesToRemove.push_back(note.pitch);
    }
    for (int i = 0; i < pitchesToRemove.size(); i++) {
        unsigned int pitch = pitchesToRemove[i];
        size_t j;
        for (j = 0; (notes[j].pitch != pitch) && j < notes.size(); j++);
        notes.erase(notes.begin() + j);
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
        float a = 0;
        for (int i = 0; i < notes.size(); i++) {
            a += notes[i].getSample(audioFrame + frame, frequencies, N);
        }
        a /= 8;
        outBuffer.getSample(frame, 0) = a;
        outBuffer.getSample(frame, 1) = a;
    }
    audioFrame += outBuffer.getNumFrames();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    if (msg.velocity > 0) {
        noteOn(msg.pitch, msg.velocity / 127.0);
    } else {
        noteOff(msg.pitch);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    int pitch = keyToPitch(key);
    if (pitch == -1) return;
    noteOn(pitch, 1.0);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    unsigned int pitch = keyToPitch(key);
    noteOff(pitch);
}

//--------------------------------------------------------------
void ofApp::noteOn(unsigned int pitch, double velocity){
    for (int i = 0; i < notes.size(); i++)
        if (notes[i].pitch == pitch && notes[i].active()) return;
    notes.push_back(Sound(pitch, velocity, audioFrame));
}

//--------------------------------------------------------------
void ofApp::noteOff(unsigned int pitch){
    for (int i = 0; i < notes.size(); i++) {
        Sound & note = notes[i];
        if (note.pitch != pitch) continue;
        if (!note.active()) continue;
        note.release(audioFrame);
        break;
    }
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
