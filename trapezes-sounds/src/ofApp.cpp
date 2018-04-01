#include "ofApp.h"

int min(int a, int b) {
    return a < b ? a : b;
}
int max(int a, int b) {
    return a > b ? a : b;
}

float pointySin(float x, float pointiness) {
    float out = sin(x);
    for (int i = 1; i <= 15; i += 2) {
        out += sin(x * i) * pow(i, -2) * pointiness;
    }
    return out;
}

void trapSound(vector <float> * samples, int sampleRate, float freq, float start, float stability, float trapAmp) {
    float f = freq * (1 + ofRandom(-1 + stability, 1 - stability));
    float attack = ATTACK * ofRandom(stability, pow(1 / stability, 2));
    float sustain = SUSTAIN * ofRandom(pow(stability, 2), pow(1 / stability, 2));
    float decay = DECAY * ofRandom(pow(stability, 2), pow(1 / stability, 2));
    float release = RELEASE * ofRandom(pow(stability, 2), pow(1 / stability, 2));
    float pointiness = ofRandom(0, 1 - pow(stability, .2));

    float phase_step = (f / sampleRate) * TWO_PI;

    // attack
    int attack_start_step = (start - attack) * sampleRate;
    int attack_end_step = start * sampleRate;
    for (int i = max(attack_start_step, 0); i < attack_end_step; i++) {
        float fade = (i - attack_start_step) / (float)(attack_end_step - attack_start_step);
        float amp = 0.8;
        (*samples)[i] += trapAmp * pointySin(i * phase_step, pointiness) * fade * amp;
    }
    
    // decay
    int decay_start_step = attack_end_step;
    int decay_end_step = decay_start_step + decay * sampleRate;
    for (int i = decay_start_step; i < min(decay_end_step, (*samples).size()); i++) {
        float fade = (decay_end_step - i) / (float)(attack_end_step - attack_start_step);
        float start_amp = 0.8;
        float end_amp = 0.6;
        (*samples)[i] += trapAmp * pointySin(i * phase_step, pointiness) * (fade * (start_amp - end_amp) + end_amp);
    }
    
    if (decay_end_step > (*samples).size()) {
        return;
    }
    
    // sustain
    int sustain_start_step = decay_end_step;
    int sustain_end_step = sustain_start_step + sustain * sampleRate;
    for (int i = sustain_start_step; i < min(sustain_end_step, (*samples).size()); i++) {
        float amp = 0.6;
        (*samples)[i] += trapAmp * pointySin(i * phase_step, pointiness) * amp;
    }

    if (sustain_end_step > (*samples).size()) {
        return;
    }

    // release
    int release_start_step = sustain_end_step;
    int release_end_step = release_start_step + release * sampleRate;
    for (int i = release_start_step; i < min(release_end_step, (*samples).size()); i++) {
        float fade = (release_end_step - i) / (float)(release_end_step - release_start_step);
        float start_amp = 0.6;
        (*samples)[i] += trapAmp * pointySin(i * phase_step, pointiness) * fade * start_amp;
    }
}

void init(vector <float> * audio) {
    (*audio).assign(44100 * 46, 0.0);  // 45ish seconds
    
    float t1 = 1;
    float t2 = 3;
    float t3 = 5;
    float t4 = 7;
    float t5 = 9;
    float t6 = 11;
    float t7 = 13;
    float t8 = 15;
    float t9 = 17;
    float t10 = 19;
    float t11 = 21;
    float t12 = 23;
    float t13 = 25;
    float t14 = 27;
    float t15 = 29;
    
    // first tone: normal
    trapSound(audio, 44100, 440, t1, 1.0, 1.0);
    
    // second tone: 1 unstable added
    trapSound(audio, 44100, 440, t2, 1.0, 0.5);
    trapSound(audio, 44100, 440, t2, 0.97, 0.5);
    
    // third tone: 2 unstable added
    trapSound(audio, 44100, 440, t3, 1.0, 0.333);
    trapSound(audio, 44100, 440, t3, 0.97, 0.333);
    trapSound(audio, 44100, 440, t3, 0.95, 0.333);
    
    // third tone: 3 unstable added
    trapSound(audio, 44100, 440, t4, 1.0, 0.25);
    trapSound(audio, 44100, 440, t4, 0.96, 0.25);
    trapSound(audio, 44100, 440, t4, 0.91, 0.25);
    trapSound(audio, 44100, 440, t4, 0.89, 0.25);
    
    // ...
    trapSound(audio, 44100, 440, t5, 1.0, 0.2);
    trapSound(audio, 44100, 440, t5, 0.95, 0.2);
    trapSound(audio, 44100, 440, t5, 0.9, 0.2);
    trapSound(audio, 44100, 440, t5, 0.85, 0.2);
    trapSound(audio, 44100, 440, t5, 0.8, 0.2);
    
    // ...
    trapSound(audio, 44100, 440, t6, 1.0, 1/6.0);
    trapSound(audio, 44100, 440, t6, 0.92, 1/6.0);
    trapSound(audio, 44100, 440, t6, 0.85, 1/6.0);
    trapSound(audio, 44100, 440, t6, 0.75, 1/6.0);
    trapSound(audio, 44100, 440, t6, 0.65, 1/6.0);
    trapSound(audio, 44100, 440, t6, 0.55, 1/6.0);
    
    // ...
    trapSound(audio, 44100, 440, t7, 1.0, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.9, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.8, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.7, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.6, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.5, 1/7.0);
    trapSound(audio, 44100, 440, t7, 0.4, 1/7.0);
    
    // ...
    trapSound(audio, 44100, 440, t8, 1.0, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.9, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.8, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.7, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.6, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.5, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.5, 1/8.0);
    trapSound(audio, 44100, 440, t8, 0.4, 1/8.0);
    
    // ...
    trapSound(audio, 44100, 440, t9, 1.0, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.9, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.8, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.7, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.6, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.5, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.5, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.4, 1/9.0);
    trapSound(audio, 44100, 440, t9, 0.4, 1/9.0);
    
    // ...
    trapSound(audio, 44100, 440, t10, 1.0, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.9, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.8, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.7, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.6, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.5, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.5, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.4, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.4, 1/10.0);
    trapSound(audio, 44100, 440, t10, 0.4, 1/10.0);
    
    // ...
    trapSound(audio, 44100, 440, t11, 1.0, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.9, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.8, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.7, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.6, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.5, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.5, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.4, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.4, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.4, 1/11.0);
    trapSound(audio, 44100, 440, t11, 0.3, 1/11.0);
    
    // ...
    trapSound(audio, 44100, 440, t12, 1.0, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.9, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.8, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.7, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.6, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.5, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.5, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.4, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.4, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.4, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.3, 1/12.0);
    trapSound(audio, 44100, 440, t12, 0.3, 1/12.0);
    
    // ...
    trapSound(audio, 44100, 440, t13, 1.0, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.9, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.8, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.7, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.6, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.5, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.5, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.4, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.4, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.4, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.3, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.3, 1/13.0);
    trapSound(audio, 44100, 440, t13, 0.3, 1/13.0);
    
    // ...
    trapSound(audio, 44100, 440, t14, 1.0, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.9, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.8, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.7, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.6, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.5, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.5, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.5, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.4, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.4, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.4, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.3, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.3, 1/14.0);
    trapSound(audio, 44100, 440, t14, 0.3, 1/14.0);
    
    // ...
    trapSound(audio, 44100, 440, t15, 1.0, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.9, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.8, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.7, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.6, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.5, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.5, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.4, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.4, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.4, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.3, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.3, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.3, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.3, 1/15.0);
    trapSound(audio, 44100, 440, t15, 0.2, 1/15.0);
}

//--------------------------------------------------------------
void ofApp::setup(){
    outStep = 0;
    ofSoundStreamSetup(2, 0, 44100, 128, 2);
    init(&audio);
    hasDrawn = false;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    hasDrawn = true;
    float amount = 0;
    for (int i = outStep; i < outStep + 0.02 * 44100; i++) {
        amount += abs(audio[i]);
    }
    ofSetBackgroundColor(amount / 3, amount / 11, amount / 5);
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer){
    if (!hasDrawn) {
        return;
    }
    for (size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        float sample = audio[outStep];
        outBuffer.getSample(i, 0) = sample / 3.0;
        outBuffer.getSample(i, 1) = sample / 3.0;
        outStep += 1;
    }
    
    if (outStep > 44100 * 40) {
        init(&audio);
        outStep = 0;
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
