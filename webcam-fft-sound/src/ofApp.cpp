#include "ofApp.h"

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }

float abs(kiss_fft_cpx s) {
    return sqrt(pow(s.r, 2) + pow(s.i, 2));
}

ofColor heat(float l) {
    return ofColor(min(l, 255.0), min(l / 4, 255.0), l > 1024 ? 255 : 0);
}

//--------------------------------------------------------------
void ofApp::setup(){
    ready = false;

    camWidth = 320;
    camHeight = 240;
    imSize = min(camWidth, camHeight);

    vidGrabber.setDeviceID(0);  // fingers crossed
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth, camHeight);

    greyPixels.allocate(imSize, imSize, OF_PIXELS_RGB);
    greyTexture.allocate(greyPixels);
    fftPixels.allocate(imSize, imSize / 2, OF_PIXELS_RGB);  // mono?
    fftTexture.allocate(fftPixels);
    
    int dims [2] = { (int)imSize, (int)imSize };
    kissNdCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    fftGreyIn = new kiss_fft_cpx[imSize * imSize];
    fftOut = new kiss_fft_cpx[imSize * imSize];

    fftFilteredPixels.allocate(imSize, imSize / 2, OF_PIXELS_RGB);
    fftFilteredTexture.allocate(fftFilteredPixels);
    maxByRadius = new angle_freq[imSize / 2];
    fftFilteredIn = new kiss_fft_cpx[imSize * imSize];
    fftFilteredOut = new kiss_fft_cpx[imSize * imSize];

    kissCfg = kiss_fft_alloc(imSize, true, 0, 0);
    fftLeftIn = new kiss_fft_cpx[N_FFT_OUT];
    fftLeftOut = new kiss_fft_cpx[N_FFT_OUT];
    fftRightIn = new kiss_fft_cpx[N_FFT_OUT];
    fftRightOut = new kiss_fft_cpx[N_FFT_OUT];

    for (size_t i = 0; i < imSize * imSize; i++) {
        fftGreyIn[i].i = 0.0;  // imaginary part is always zero
    }
    for (size_t i = imSize; i < N_FFT_OUT; i++) {
        fftLeftIn[i].r = fftLeftIn[i].i = fftLeftOut[i].r = fftLeftOut[i].i = 0;
        fftRightIn[i].r = fftRightIn[i].i = fftRightOut[i].r = fftRightOut[i].i = 0;
    }

    ofSoundStreamSetup(2, 0, 44100, 256, 2);
    ofSetVerticalSync(true);
    ready = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (!ready) return;
    ofBackground(0);
    vidGrabber.update();

    if (vidGrabber.isFrameNew() && ready) {  // boo races
        ofPixels & pixels = vidGrabber.getPixels();
        pixels.mirror(false, true);
        
        for (int x = 0; x < imSize; x++) {
            for (int y = 0; y < imSize; y++) {
                double r = sqrt(pow((int)imSize / 2 - x, 2) + pow((int)imSize / 2 - y, 2));
                ofColor c = pixels.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
                double w = r <= (imSize / 2)  // radial hanning
                    ? 0.5 - 0.5 * cos(PI * (1 - r / ((double)imSize / 2)))
                    : 0;
                float l = c.getLightness() * w;
                greyPixels.setColor(x, y, ofColor(l));
                int shift = pow(-1, x + y);  // centre the fft
                fftGreyIn[y * imSize + x].r = ofMap(l, 0, 255, -1, 1) * shift;
            }
        }

        greyTexture.loadData(greyPixels);
        kiss_fftnd(kissNdCfg, fftGreyIn, fftOut);
        
        for (int i = 0; i < imSize; i++) {
            if (i < imSize / 2) {
                maxByRadius[i].cpx.r = maxByRadius[i].cpx.i = 0;
            }
            for (int j = 0; j < imSize; j++) {
                fftFilteredIn[i].r = fftFilteredIn[i].i = 0;
                if (j < imSize / 2) {
                    fftFilteredPixels.setColor(i, j, 0);
                }
            }
        }
    
        for (int i = 0; i < N_FFT_OUT; i++) {
            fftLeftIn[i].r = fftLeftIn[i].i = fftRightIn[i].r = fftRightIn[i].i = 0;
        }
        double minT = INFINITY, maxT = -INFINITY;

        for (size_t x = 0; x < imSize; x++) {
            for (size_t y = 0; y < imSize / 2; y++) {
                int xSpecOrigin = x - imSize / 2;
                int ySpecOrigin = imSize / 2 - y;
                int ring = sqrt(pow(xSpecOrigin, 2) + pow(ySpecOrigin, 2));
                if (ring < 1) {
                    fftPixels.setColor(x, y, ofColor(0));
                } else if (ring < imSize / 2) {
                    size_t outI = y * imSize + x;
                    kiss_fft_cpx out = fftOut[outI];

                    float l = abs(out) * 4;
                    fftPixels.setColor(x, y, heat(l));

                    double theta = xSpecOrigin > 0
                        ? atan(ySpecOrigin / (xSpecOrigin == 0 ? 0.00001 : (double)xSpecOrigin))
                        : PI - atan(ySpecOrigin / (xSpecOrigin == 0 ? 0.00001 : abs((double)xSpecOrigin)));

                    minT = min(minT, theta);
                    maxT = max(maxT, theta);

                    if (abs(out) > abs(maxByRadius[ring].cpx)) {
                        maxByRadius[ring].angle = theta;
                        maxByRadius[ring].cpx = out;
                    }

                    double pan = theta / PI;  // half turn

                    double real = out.r / (ring * PI);
                    double imaj = out.i / (ring * PI);

                    fftLeftIn[ring].r += (1 - pan) * real;
                    fftLeftIn[ring].i += (1 - pan) * imaj;
                    fftLeftIn[N_FFT_OUT - ring].r += (1 - pan) * -real;
                    fftLeftIn[N_FFT_OUT - ring].i += (1 - pan) * -imaj;

                    fftRightIn[ring].r += pan * real;
                    fftRightIn[ring].i += pan * imaj;
                    fftRightIn[N_FFT_OUT - ring].r += pan * -real;
                    fftRightIn[N_FFT_OUT - ring].i += pan * -imaj;
                } else {
                    fftPixels.setColor(x, y, 0);
                }
            }
        }
        cout << "min, max: " << minT << ", " << maxT << endl;
        
        fftTexture.loadData(fftPixels);
        
        for (int radius = 0; radius < imSize / 2; radius++) {
            angle_freq f = maxByRadius[radius];
            int y = radius * sin(f.angle);
            int x = radius * cos(f.angle);
            ofColor c = heat(abs(f.cpx) * 4);
            fftFilteredPixels.setColor(imSize / 2 + x, imSize / 2 - y, c);
            fftFilteredIn[(imSize - y) * imSize + x + imSize / 2] = f.cpx;
        }
        fftFilteredTexture.loadData(fftFilteredPixels);
        
//        kiss_fft(kissCfg, fftLeftIn, fftLeftOut);
//        kiss_fft(kissCfg, fftRightIn, fftRightOut);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!ready) return;
    ofSetColor(0xFF);
//    vidGrabber.draw(20, 20, camWidth, camHeight);
    greyTexture.draw(20, 20 + camHeight + 20, imSize, imSize);
//    float imDiag = sqrt(2 * pow(imSize / 2, 2));

    ofPushMatrix();
    ofTranslate(20 + imSize + 20, 20 + camHeight + 20);
//    ofRotate(-180);
    fftTexture.draw(0, 0, imSize, imSize / 2);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(20 + imSize + 20, 20 + camHeight + 20 + imSize / 2);
    fftFilteredTexture.draw(0, 0, imSize, imSize / 2);
    ofPopMatrix();

//    ofPolyline leftSpectrum;
//    ofPolyline rightSpectrum;
//    for (int i = 0; i < N_FFT_OUT; i++) {
//        float left = abs(fftLeftIn[i]);
//        leftSpectrum.addVertex(ofPoint(i / 2.0, -left));
//        float right = abs(fftRightIn[i]);
//        rightSpectrum.addVertex(ofPoint(i / 2.0, -right));
//    }
//
//    ofPushMatrix();
//    ofSetHexColor(0xFFFF00);
//    ofTranslate(20 + imSize + 20 + imSize + 20, 20 + imSize + 20 + imSize / 2);
//    leftSpectrum.draw();
//    ofDrawLine(0, 0, imSize / 4, 0);
//    ofPopMatrix();
//
//    ofPushMatrix();
//    ofSetHexColor(0xFF0000);
//    ofTranslate(20 + imSize + 20 + imSize + 20, 20 + imSize + 20 + imSize);
//    rightSpectrum.draw();
//    ofDrawLine(0, 0, imSize / 4, 0);
//    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer) {
    uint samplesSince = 0;
    for (int i = 0; i < outBuffer.getNumFrames(); i++) {
        float left = fftLeftOut[(audioStep + i) % N_FFT_OUT].r;
        float right = fftRightOut[(audioStep + i) % N_FFT_OUT].r;
        outBuffer.getSample(i, 0) = ofMap(left, -10000, 10000, -1, 1, true);
        outBuffer.getSample(i, 1) = ofMap(right, -10000, 10000, -1, 1, true);
    }
    audioStep += outBuffer.getNumFrames();
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
