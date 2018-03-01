#include "ofApp.h"

float pointySin(float x, vector <float> harmonics) {
    float out = 0.0;
    for (int i = 0; i < harmonics.size(); i++) {
        float f = x * (i + 1);
        float a = harmonics[i];
        out += sin(f) * a;
    }
    return out;
}

//--------------------------------------------------------------
void ofApp::setup(){
    midiIn.listPorts(); // via instance
    midiIn.openPort(2);
    midiIn.addListener(this);

    ofSoundStreamSetup(2, 0);

    step = 0;
    amp = 1;
    freq = 440.0;
    harmonicAmps.assign(8, 0.0);
    harmonicAmps[0] = 0.2;
    keys_down_count = 0;
    
    for (int i = 0; i <= 108; i++) {  // 88-key midi note range max
        float f = pow(2, (i - 69) / 12.0) * 440;
        freqs.push_back(f);
    }
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    float px_step = 1.0 / ofGetWidth() * TWO_PI;
    ofPolyline wave;
    for (int x = -10; x < ofGetWidth() + 10; x++) {
        float signal = pointySin(x * px_step, harmonicAmps);
        wave.addVertex(x, ofMap(signal, -1, 1, 3 * ofGetHeight() / 4, ofGetHeight() / 4));
    }
    ofSetLineWidth(2);
    wave.draw();
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    if (msg.channel == 1) {  // keystation
        if (msg.velocity == 0) {
            handleNoteOff(msg.pitch);
        } else {
            handleNoteOn(msg.pitch, msg.velocity);
        }
    } else if (msg.channel == 2) {  // kontrol
        if (msg.control <= 15) {
            handleSlider(msg.control, msg.value);
        } else if (32 <= msg.control && msg.control <= 39) {
            handleSolo(msg.control % 8);
        } else if (48 <= msg.control && msg.control <= 55) {
            handleMute(msg.control % 8);
        } else {
            cout << msg.control << endl;
        }
    }
}

//--------------------------------------------------------------
void ofApp::handleNoteOff(int pitch) {
    keys_down_count -= 1;
    if (keys_down_count <= 0) {
        amp = 0.0;
        keys_down_count = 0;
    }
}

//--------------------------------------------------------------
void ofApp::handleNoteOn(int pitch, int velocity) {
    keys_down_count += 1;
    freq = pow(2, (pitch - 69) / 12.0) * 440;
    amp = ofMap(velocity, 0, 127, 0, 1);
}

//--------------------------------------------------------------
void ofApp::handleSlider(int slider, int value) {
    float range = 1.0 / (1 + slider);
    harmonicAmps[slider] = ofMap(value, 0, 127, -range, range);
}

//--------------------------------------------------------------
void ofApp::handleSolo(int slider) {
    for (int i = 0; i < 8; i++) {
        if (i == slider) continue;
        harmonicAmps[i] = 0.0;
    }
}

//--------------------------------------------------------------
void ofApp::handleMute(int slider) {
    harmonicAmps[slider] = 0.0;
}

void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    float phase_step = freq / outBuffer.getSampleRate() * TWO_PI;
    for (int i = 0; i < outBuffer.getNumFrames(); i++) {
        float sample = pointySin((step + i) * phase_step, harmonicAmps) * amp;
        outBuffer.getSample(i, 0) = sample;
        outBuffer.getSample(i, 1) = sample;
    }
    step += outBuffer.getNumFrames();
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
