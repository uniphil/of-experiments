#include "ofApp.h"

double abs(kiss_fft_cpx p) {
    return sqrt(pow(p.r, 2) + pow(p.i, 2));
}

void prepareImageForFFT(ofPixels image, int imSize, ofPixels & masked, kiss_fft_cpx * out) {
    if (image.getHeight() < imSize || image.getWidth() < imSize) {
        ofLog(OF_LOG_ERROR, "imSize is smaller image");
    }
    int h = image.getHeight();
    int w = image.getWidth();
    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            int imX = x + (w - imSize) / 2;
            int imY = y + (h - imSize) / 2;
            double r = sqrt(pow(imSize / 2 - x, 2) + pow(imSize / 2 - y, 2));
            double w = r <= (imSize / 2)  // radial hanning
            ? 0.5 - 0.5 * cos(PI * (1 - r / ((double)imSize / 2)))
            : 0;
            double shift = pow(-1, x + y);  // centre the fft
            double v = image.getColor(imX, imY).getLightness();
            masked.setColor(x, y, ofColor(v, v, v, w * 0xFF));
            out[y * imSize + x].i = 0;
            out[y * imSize + x].r = v * w * shift;
        }
    }
}

void getFreqs(kiss_fft_cpx * imFreqs, int imSize, ofPixels & out) {
    for (int x = 0; x < imSize; x++) {
        for (int y = 0; y < imSize; y++) {
            double v = abs(imFreqs[y * imSize + x]) / pow(imSize, 2);
            out.setColor(x, y, ofColor(0xFF, 0xFF, 0xFF, ofMap(v, 0, 1, 0, 255, true)));
        }
    }
}

void linearize(kiss_fft_cpx * imFreqs, int imSize, double angle, double * out) {
    for (int i = 0; i < imSize / 2; i++) {
        int x = round(imSize / 2 + i * cos(angle));
        int y = round(imSize / 2 + i * sin(angle));
        out[i] = sqrt(abs(imFreqs[y * imSize + x]));  // sqrt: we're going from square to linear
    }
    cout << endl;
}

//--------------------------------------------------------------
void ofApp::setup(){
    imSize = 240;
    grabber.initGrabber(320, 240);
    grabber.update();

    last = 0;

    int dims [2] = {(int)imSize, (int)imSize};
    webcamToFreq = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    image = new kiss_fft_cpx[imSize * imSize];
    imFreqs = new kiss_fft_cpx[imSize * imSize];

    masked.allocate(imSize, imSize, OF_IMAGE_COLOR_ALPHA);
    transformed.allocate(imSize, imSize, OF_IMAGE_COLOR_ALPHA);

    linFreqAmps = new double[imSize / 2];
    linFreqPhases = new double[imSize / 2];
    for (int i = 0; i < imSize / 2; i++) linFreqPhases[i] = 0;

    ofSoundStreamSetup(2, 0, F, S, 2);
    
    angle = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    grabber.update();
    float now = ofGetElapsedTimef();
    if (grabber.isFrameNew() && now > last + 0.1) {
        last = now;
        processFrame(grabber.getPixels());
    }
    linearize(imFreqs, imSize, angle, linFreqAmps);
    angle = ofGetElapsedTimeMillis() / 1000.0 * TWO_PI / 1;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0x00);

    // webcam preview
//    ofSetColor(0x22, 0x00, 0x11);
//    grabber.draw(0, 0);

    // windowed image preview
    ofSetColor(0x88, 0x00, 0x44);
    masked.draw(grabber.getWidth() / 2 - imSize / 2, grabber.getHeight() / 2 - imSize / 2);
    ofSetColor(0xFF, 0xFF, 0x00);
    transformed.draw(grabber.getWidth() / 2 - imSize / 2, grabber.getHeight() / 2 - imSize / 2);

    // draw the line that's being taken
    // (it's from the fft, not the image, but, eh)
    ofSetColor(0xFF, 0x00, 0x88, 0x88);
    ofDrawLine(grabber.getWidth() / 2 - imSize / 2 * cos(angle),
               grabber.getHeight() / 2 - imSize / 2 * sin(angle),
               grabber.getWidth() / 2 + imSize / 2 * cos(angle),
               grabber.getHeight() / 2 + imSize / 2 * sin(angle));

    // what's the linearized thing looking like?
    ofPushMatrix();
    ofTranslate(grabber.getWidth() + 20, 0);
    ofPolyline freqAmpY;
    for (int i = 0; i < imSize / 2; i++) {
        freqAmpY.addVertex(i * 2, imSize - linFreqAmps[i]);
    }
    ofSetColor(0xFF, 0x00, 0x88, 0x88);
    freqAmpY.draw();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    double dt = 1.0 / outBuffer.getSampleRate();
    for (size_t frame = 0; frame < outBuffer.getNumFrames(); frame++) {
        double a = 0;
        for (int i = 0; i < imSize / 2; i++) {
            if (i == 0) continue;  // ignore dc offset (or make it a global amplifier?)
            double f = i * 40;  // 20 hz / bin, arbitrarily
            linFreqPhases[i] += dt * f * TWO_PI;
            if (linFreqPhases[i] > TWO_PI) linFreqPhases[i] -= TWO_PI;
            a += linFreqAmps[i] * sin(linFreqPhases[i]);
        }

        a /= imSize * 100;  // try to keep things from getting too wild??
        if (a > 1) {
            cout << "WATTTT " << a << endl;
            continue;
        }
        outBuffer.getSample(frame, 0) = a;
        outBuffer.getSample(frame, 1) = a;
    }
}

//--------------------------------------------------------------
void ofApp::processFrame(ofPixels & imagePixels) {
    prepareImageForFFT(imagePixels, imSize, masked.getPixels(), image);
    masked.update();
    kiss_fftnd(webcamToFreq, image, imFreqs);
    getFreqs(imFreqs, imSize, transformed.getPixels());
    transformed.update();
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
