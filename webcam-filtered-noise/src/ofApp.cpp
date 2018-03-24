#include "ofApp.h"

int min(int a, int b) { return a < b ? a : b; }

void scale(kiss_fft_cpx & cpx, double x) {
    cpx.r *= x;
    cpx.i *= x;
}

void scale(kiss_fft_cpx & cpx, kiss_fft_cpx s) {
    double dot = cpx.r * s.r + cpx.i * s.i;
    scale(cpx, dot);
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

void setNoise(float * staticNoise, int imSize) {
    for (int i = 0; i < imSize * imSize; i++) {
        staticNoise[i] = ofRandom(-1, 1);
    }
}

//--------------------------------------------------------------
void ofApp::setup(){
    ready = false;
    
    camWidth = 600;
    camHeight = 400;
    imSize = min(camWidth, camHeight);

    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(camWidth, camHeight);

    allocate(grey, imSize, imSize);
    allocate(noise, imSize, imSize);
    allocate(out, imSize, imSize);

    int dims [2] = { (int)imSize, (int)imSize };
    forwardCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    inverseCfg = kiss_fftnd_alloc(dims, 2, true, 0, 0);
    greySamples = new kiss_fft_cpx[imSize * imSize];
    greyFreqs = new kiss_fft_cpx[imSize * imSize];
    noiseSamples = new kiss_fft_cpx[imSize * imSize];
    noiseFreqs = new kiss_fft_cpx[imSize * imSize];
    outSamples = new kiss_fft_cpx[imSize * imSize];
    
    staticNoise = new float[imSize * imSize];
    setNoise(staticNoise, imSize);

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

    float amount = ofMap(mouseX, 0, ofGetWidth(), 0, 1, true);

    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            ofColor c = source.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
            double l = c.getLightness();
            double r = ofDist(imSize / 2, imSize / 2, x, y);
            double w = r <= (imSize / 2)  // radial hanning
                ? 0.5 - 0.5 * cos(PI * (1 - r / ((double)imSize / 2)))
                : 0;
            l *= w;  // apply window
//            grey.pix.setColor(x, y, l);
            l = ofMap(l, 0, 255, -1, 1);
            l *= pow(-1, x + y);  // centre fft
            greySamples[y * imSize + x].i = 0;
            greySamples[y * imSize + x].r = l;

            double rl = ofRandom(-amount, amount);
//            double rl = staticNoise[y * imSize + x];
//            noise.pix.setColor(x, y, ofMap(rl, -1, 1, 0, 255));
            noiseSamples[y * imSize + x].i = 0;
            noiseSamples[y * imSize + x].r = rl;
        }
    }

//    load(grey);
    kiss_fftnd(forwardCfg, greySamples, greyFreqs);
    kiss_fftnd(forwardCfg, noiseSamples, noiseFreqs);

    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize / 2; y++) {
            int i = y * imSize + x;
            int ii = imSize * imSize - i;
            kiss_fft_cpx & p = noiseFreqs[i];
            kiss_fft_cpx & pi = noiseFreqs[ii];

            scale(p, greyFreqs[i]);
            scale(pi, greyFreqs[ii]);

//            rotate(p, r * PI);
//            rotate(pi, r * PI);
//            fft.pix.setColor(x, y, ofMap(abs(p), 0, imSize, 0, 255, true));
//            fft.pix.setColor(imSize - x, imSize - y, ofMap(abs(pi), 0, imSize, 0, 255, true));
        }
    }

//    load(fft);
    kiss_fftnd(inverseCfg, noiseFreqs, outSamples);
 
    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            double l = outSamples[y * imSize + x].r;
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
    int size = min(ofGetWidth(), ofGetHeight());
    int x = ofGetWidth() / 2 - size / 2;
    int y = ofGetHeight() / 2 - size / 2;
//    grey.tex.draw(0, y);
//    noise.tex.draw(0, imSize);
//    fft.tex.draw(imSize, y);
    out.tex.draw(x, y, size, size);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    setNoise(staticNoise, imSize);
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
