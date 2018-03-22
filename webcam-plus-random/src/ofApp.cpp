#include "ofApp.h"

int min(int a, int b) { return a < b ? a : b; }

void scale(kiss_fft_cpx & cpx, double x) {
    cpx.r *= x;
    cpx.i *= x;
}

void rotate(kiss_fft_cpx & cpx, double theta) {
    double cosT = cos(theta);
    double sinT = sin(theta);
    float rPrime = cpx.r * cosT - cpx.i * sinT;
    float iPrime = cpx.r * sinT + cpx.i * cosT;
    cpx.r = rPrime;
    cpx.i = iPrime;
}

float abs(kiss_fft_cpx cpx) {
    return sqrt(pow(cpx.r, 2) + pow(cpx.i, 2));
}

void allocate(im & i, size_t w, size_t h) {
    i.pix.allocate(w, h, OF_PIXELS_RGB);
    i.tex.allocate(i.pix);
}

void load(im & i) {
    i.tex.loadData(i.pix);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ready = false;
    
    camWidth = 640;
    camHeight = 480;
    imSize = min(camWidth, camHeight);

    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(camWidth, camHeight);

    allocate(grey, imSize, imSize);
    allocate(fft, imSize, imSize);
    allocate(out, imSize, imSize);

    int dims [2] = { (int)imSize, (int)imSize };
    forwardCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    inverseCfg = kiss_fftnd_alloc(dims, 2, true, 0, 0);
    spaceSamples = new kiss_fft_cpx[imSize * imSize];
    freqSamples = new kiss_fft_cpx[imSize * imSize];

    ofSetVerticalSync(true);
    ready = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!ready) return;
    vidGrabber.update();
    if (!vidGrabber.isFrameNew()) return;

    ofPixels & source = vidGrabber.getPixels();
    source.mirror(false, true);

    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            ofColor c = source.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
            double l = c.getLightness();
            double r = ofDist(imSize / 2, imSize / 2, x, y);
            double w = r <= (imSize / 2)  // radial hanning
                ? 0.5 - 0.5 * cos(PI * (1 - r / ((double)imSize / 2)))
                : 0;
            l *= w;  // apply window
            grey.pix.setColor(x, y, l);
            l = ofMap(l, 0, 255, -1, 1);
            l *= pow(-1, x + y);  // centre fft
            spaceSamples[y * imSize + x].i = 0;
            spaceSamples[y * imSize + x].r = l;
        }
    }

    load(grey);
    kiss_fftnd(forwardCfg, spaceSamples, freqSamples);
    
    float randRange = ofMap(mouseX, 0, ofGetWidth(), 0, 1, true);

    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize / 2; y++) {
            int i = y * imSize + x;
            int ii = imSize * imSize - i;
            kiss_fft_cpx & p = freqSamples[i];
            kiss_fft_cpx & pi = freqSamples[ii];
            float r = ofRandom(-randRange, randRange);
//            scale(p, 1 + r * 5);
//            scale(pi, 1 + r * 5);
            rotate(p, r * PI);
            rotate(pi, r * PI);
            fft.pix.setColor(x, y, ofMap(abs(p), 0, imSize, 0, 255, true));
            fft.pix.setColor(imSize - x, imSize - y, ofMap(abs(pi), 0, imSize, 0, 255, true));
        }
    }

    load(fft);
    kiss_fftnd(inverseCfg, freqSamples, spaceSamples);
 
    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            double l = spaceSamples[y * imSize + x].r;
            l *= pow(-1, x + y);  // un-centre
            l /= imSize * imSize;
            out.pix.setColor(x, y, ofMap(l, -1, 1, 0, 255, true));
        }
    }

    load(out);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    int y = ofGetHeight() / 2 - imSize / 2;
    grey.tex.draw(0, y);
    fft.tex.draw(imSize, y);
    out.tex.draw(imSize * 2, y);
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
