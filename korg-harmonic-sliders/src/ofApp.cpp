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

float envelope(float age, float death, float attack, float sustain, float release) {
    float out = 1.0;
    if (age < 0) return 0;  // wat
    if (age < attack) {
        out = age / attack;
    } else {
        float t = age - attack;
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
    midiIn.listPorts();
    midiIn.openPort(2);  // use MidiPipe to map keystation -> chan 1, kontrol -> 2
    midiIn.addListener(this);
    ofSoundStreamSetup(2, 0, 44100, 128, 2);
//    ofSoundStreamSetup(2, 0);

    step = 0;
    harmonicAmps.assign(8, 0.0);
    harmonicAmps[0] = 0.667;
    harmonicAmps[1] = 0.333;
    attack = 0.06;
    sustain = 0.2;
    release = 0.2;

    for (int i = 0; i <= 108; i++) {  // 88-key midi note range max
        float f = pow(2, (i - 69) / 12.0) * 440;
        freqs.push_back(f);
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    vector <unsigned int> pitches_to_remove;
    for (int i = 0; i < notes.size(); i++) {
        Sound note = notes[i];
        if (note.died && ((step - note.died) / 44100.0 > release * 1.1)) {
            pitches_to_remove.push_back(note.pitch);
        }
    }
    for (int i = 0; i < pitches_to_remove.size(); i++) {
        unsigned int pitch = pitches_to_remove[i];
        size_t j;
        for (j = 0; (notes[j].pitch != pitch) && j < notes.size(); j++);
        notes.erase(notes.begin() + j);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    int octs = ofGetHeight() / 8;

    ofSetBackgroundColor(0x44);
    ofSetLineWidth(3);
    
    float sus_duration = 1.0;
    float px_step = 1.0 / (2 * ofGetWidth() / 3) * (attack + sus_duration + release);
    ofPolyline env;
    for (int x = 0; x < 2 * ofGetWidth() / 3; x++) {
        float amp = envelope(x * px_step, attack + sus_duration, attack, sustain, release);
        env.addVertex(x + ofGetWidth() / 6, ofMap(amp, 0, 1, 3 * octs, 1 * octs));
    }
    ofSetColor(0x55, 0x55, 0x55);
    ofDrawLine(-10, 3 * octs, ofGetWidth() / 6, 3 * octs);
    ofDrawLine(5 * ofGetWidth() / 6, 3 * octs, ofGetWidth() + 10, 3 * octs);
    ofSetColor(0xCC, 0x88, 0xFF);
    env.draw();

    px_step = 2.0 / ofGetWidth() * TWO_PI;  // two whole cycles
    // left of wave
    ofPolyline left, wave, right;
    for (int x = -10; x < ofGetWidth() + 10; x++) {
        float signal = pointySin(x * px_step, harmonicAmps);
        float y = ofMap(signal, -1, 1, 7.5 * octs, 4.5 * octs);
        ofPolyline segment;
        if (x < ofGetWidth() / 4) {
            left.addVertex(x, y);
        } else if (x <= 3 * ofGetWidth() / 4 + 2) {
            wave.addVertex(x, y);
        } else {
            right.addVertex(x, y);
        }
    }

    ofSetColor(0x55, 0x55, 0x55);
    left.draw();
    right.draw();
    ofSetColor(0xFF, 0x88, 0xCC);
    wave.draw();
}

//---------------------------------`-----------------------------
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
        } else if (msg.control <= 23) {
            handleKnob(msg.control % 8, msg.value);
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
    size_t i;
    for (i = 0; (notes[i].pitch != pitch || notes[i].died) && i < notes.size(); i++);
    notes[i].died = step;
}

//--------------------------------------------------------------
void ofApp::handleNoteOn(int pitch, int velocity) {
    float vel = ofMap(velocity, 0, 127, 0, 1);
    notes.push_back(Sound {
        (unsigned int)pitch,
        vel,
        step,
        0,
    });
}

//--------------------------------------------------------------
void ofApp::handleSlider(int slider, int value) {
    float range = 1.0 / (1 + slider);
    harmonicAmps[slider] = ofMap(value, 0, 127, -range, range);
}

//--------------------------------------------------------------
void ofApp::handleKnob(int slider, int value) {
    if (slider == 0) {
        attack = pow(ofMap(value, 0, 127, 0, 1.0), 2);
    } else if (slider == 1) {
        sustain = ofMap(value, 127, 0, 0, 1);
    } else if (slider == 2) {
        release = pow(ofMap(value, 127, 0, 0, 1.0), 2.4) * 2;
    }
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
    for (int i = 0; i < notes.size(); i++) {
        Sound note = notes[i];
        float amp = note.velocity;
        float phase_step = freqs[note.pitch] / outBuffer.getSampleRate() * TWO_PI;
        for (int i = 0; i < outBuffer.getNumFrames(); i++) {
            uint64_t s = step + i;
            float age = (s - note.started) / (float)outBuffer.getSampleRate();
            float death = note.died == 0 ? 0 : (note.died - note.started) / (float)outBuffer.getSampleRate();
            float env = envelope(age, death, attack, sustain, release);
            float sample = pointySin(s * phase_step, harmonicAmps) * amp * env * 0.3;
            outBuffer.getSample(i, 0) += sample;
            outBuffer.getSample(i, 1) += sample;
        }
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
