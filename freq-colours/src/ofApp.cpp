#include "ofApp.h"

double min(double a, double b) { return a < b ? a : b; }

kiss_fft_cpx scale(kiss_fft_cpx p, double amount) {
    kiss_fft_cpx out;
    out.r = p.r * amount;
    out.i = p.i * amount;
    return out;
}

//--------------------------------------------------------------
void ofApp::setup(){
    camera.setup(W, H);
    imSize = min(camera.getWidth(), camera.getHeight());
    windowed.allocate(imSize, imSize, OF_IMAGE_GRAYSCALE);
    result.allocate(imSize, imSize, OF_IMAGE_COLOR);

    int dims [2] = { (int)imSize, (int)imSize };
    forward = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    inverse = kiss_fftnd_alloc(dims, 2, true, 0, 0);
    grey = new kiss_fft_cpx[imSize * imSize];
    freqs = new kiss_fft_cpx[imSize * imSize];
    freqsOut = new kiss_fft_cpx[imSize * imSize];
    freqsR = new kiss_fft_cpx[imSize * imSize];
    freqsG = new kiss_fft_cpx[imSize * imSize];
    freqsB = new kiss_fft_cpx[imSize * imSize];
    outR = new kiss_fft_cpx[imSize * imSize];
    outG = new kiss_fft_cpx[imSize * imSize];
    outB = new kiss_fft_cpx[imSize * imSize];
}

//--------------------------------------------------------------
void ofApp::update(){
    camera.update();
    if (!camera.isFrameNew()) return;
    
    camera.getPixels().mirror(false, true);
    for (int y = 0; y < imSize; y++) {
        for (int x = 0; x < imSize; x++) {
            float l = camera
                .getPixels()
                .getColor(camera.getWidth() / 2 - (int)imSize / 2 + x,
                          camera.getHeight() / 2 - (int)imSize / 2 + y)
                .getBrightness();
            float r = sqrt(pow((int)imSize / 2 - x, 2) + pow((int)imSize / 2 - y, 2));
            float w = r <= (imSize / 2)  // radial hanning
                ? 0.5 - 0.5 * cos(PI * (1 - r / (imSize / 2.0)))
                : 0;
            l *= w;
            windowed.setColor(x, y, l);
            l *= pow(-1, x + y);
            grey[y * imSize + x].r = l;
            grey[y * imSize + x].i = 0;

            result.setColor(x, y, 0);
        }
    }
    windowed.update();

    kiss_fftnd(forward, grey, freqs);
    
    int cycles = ofMap(mouseX, 0, ofGetWidth(), 1, 500, true) * 2;

    double slice = TWO_PI / 6;
    for (int y = 0; y < imSize; y++) {
        for (int x = 0; x < imSize; x++) {
            double theta = atan2(-y + (int)imSize / 2, x - (int)imSize / 2) * cycles;
            theta = fmod(theta + TWO_PI, TWO_PI);
            double r, g, b;
            if (theta < 1 * slice) {
                r = ofMap(theta, 0 * slice, 1 * slice, 0, 1);
                g = 0;
                b = 1;
            } else if (theta < 2 * slice) {
                r = 1;
                g = 0;
                b = ofMap(theta, 1 * slice, 2 * slice, 1, 0);
            } else if (theta < 3 * slice) {
                r = 1;
                g = ofMap(theta, 2 * slice, 3 * slice, 0, 1);
                b = 0;
            } else if (theta < 4 * slice) {
                r = ofMap(theta, 3 * slice, 4 * slice, 1, 0);
                g = 1;
                b = 0;
            } else if (theta < 5 * slice) {
                r = 0;
                g = 1;
                b = ofMap(theta, 4 * slice, 5 * slice, 0, 1);
            } else if (theta <= 6 * slice) {
                r = 0;
                g = ofMap(theta, 5 * slice, 6 * slice, 1, 0);
                b = 1;
            }
            size_t i = y * imSize + x;
            freqsR[i] = scale(freqs[i], r);
            freqsG[i] = scale(freqs[i], g);
            freqsB[i] = scale(freqs[i], b);
        }
    }

    kiss_fftnd(inverse, freqsR, outR);
    kiss_fftnd(inverse, freqsG, outG);
    kiss_fftnd(inverse, freqsB, outB);
    for (int y = 0; y < imSize; y++) {
        for (int x = 0; x < imSize; x++) {
            size_t i = y * imSize + x;
            double scale = 1 / pow(imSize, 2) * pow(-1, x + y);
            double r = outR[i].r * scale,
                   g = outG[i].r * scale,
                   b = outB[i].r * scale;
//            v /= pow(imSize, 2);
//            v *= pow(-1, x + y);
            result.setColor(x, y, ofColor(r, g, b));
        }
    }
    result.update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(255);
//    windowed.draw(0, 0);
    int smaller = min(ofGetHeight(), ofGetWidth());
    result.draw(ofGetWidth() / 2 - smaller / 2, ofGetHeight() / 2 - smaller / 2, smaller, smaller);
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
