#include "ofApp.h"

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }

float abs(kiss_fft_cpx s) {
    return sqrt(pow(s.r, 2) + pow(s.i, 2));
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
    fftPixels.allocate(imSize / 2, imSize / 2, OF_PIXELS_RGB);  // mono?
    fftTexture.allocate(fftPixels);
    
    int dims [2] = { (int)imSize, (int)imSize };
    kissNdCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    fftGreyIn = new kiss_fft_cpx[imSize * imSize];
    fftOut = new kiss_fft_cpx[imSize * imSize];

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
    ofBackground(0);
    vidGrabber.update();

    if (vidGrabber.isFrameNew() && ready) {  // boo races
        ofPixels & pixels = vidGrabber.getPixels();
        pixels.mirror(false, true);
        
        for (int x = 0; x < imSize; x++) {
            for (int y = 0; y < imSize; y++) {
                double r = sqrt(pow((int)imSize / 2 - x, 2) + pow((int)imSize / 2 - y, 2));
                ofColor c = pixels.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
                if (r > imSize) {
                    cout << "wat " << x << ", " << y << ", " << r << endl;
                }
                double w = r <= (imSize / 2)  // radial hanning
                    ? 0.5 - 0.5 * cos(PI * (1 - r / ((double)imSize / 2)))
                    : 0;
                float l = c.getLightness() * w;
                greyPixels.setColor(x, y, ofColor(l));
                fftGreyIn[y * imSize + x].r = ofMap(l, 0, 255, -1, 1);
            }
        }

        greyTexture.loadData(greyPixels);
        kiss_fftnd(kissNdCfg, fftGreyIn, fftOut);
    
        for (int i = 0; i < imSize; i++) {
            fftLeftIn[i].r = fftLeftIn[i].i = fftRightIn[i].r = fftRightIn[i].i = 0;
        }

        for (size_t x = 0; x < imSize / 2; x++) {
            for (size_t y = 0; y < imSize / 2; y++) {
                int ring = sqrt(pow(x, 2) + pow(y, 2));
                if (ring < 7) {
                    fftPixels.setColor(x, y, ofColor(0x44, 0x00, 0xcc));
                } else if (ring < imSize / 2) {
                    size_t outI = y * imSize + x;
                    kiss_fft_cpx out = fftOut[outI];

                    float l = abs(out) * 4;
                    fftPixels.setColor(x, y, ofColor(min(l, 255.0), min(l / 4, 255.0), l > 1024 ? 255 : 0));

                    double theta = asin(y / (ring ? ring : 1));
                    double pan = theta / (PI / 2);  // quarter turn

                    double real = out.r / (ring * PI / 2);
                    double imaj = out.i / (ring * PI / 2);

                    fftLeftIn[ring].r += (1 - pan) * real;
                    fftLeftIn[ring].i += (1 - pan) * imaj;
                    fftLeftIn[imSize - ring].r += (1 - pan) * real;
                    fftLeftIn[imSize - ring].i += (1 - pan) * imaj;

                    fftRightIn[ring].r += pan * real;
                    fftRightIn[ring].i += pan * imaj;
                    fftRightIn[imSize - ring].r += pan * real;
                    fftRightIn[imSize - ring].i += pan * imaj;
                } else {
                    fftPixels.setColor(x, y, 0);
                }
            }
        }
        
        kiss_fft(kissCfg, fftLeftIn, fftLeftOut);
        kiss_fft(kissCfg, fftRightIn, fftRightOut);

        fftTexture.loadData(fftPixels);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0xFF);
//    vidGrabber.draw(20, 20, camWidth, camHeight);
    greyTexture.draw(20, 20 + camHeight + 20, imSize, imSize);
    float imDiag = sqrt(2 * pow(imSize / 2, 2));

    ofPushMatrix();
    ofTranslate(20 + imSize + 20 + imDiag / 2, 20 + camHeight + 20 + imDiag);
    ofRotate(-135);
    fftTexture.draw(0, 0, imSize / 2, imSize / 2);
    ofPopMatrix();

    ofPolyline leftSpectrum;
    ofPolyline rightSpectrum;
    for (int i = 0; i < imSize / 2; i++) {
        float left = abs(fftLeftIn[i]);
        leftSpectrum.addVertex(ofPoint(i, -left));
        float right = abs(fftRightIn[i]);
        rightSpectrum.addVertex(ofPoint(i, -right));
    }

    ofPushMatrix();
    ofSetHexColor(0xFFFF00);
    ofTranslate(20 + imSize + 20 + imDiag + 20, 20 + imSize + 20 + imDiag / 2);
    leftSpectrum.draw();
    ofDrawLine(0, 0, imSize / 2, 0);
    ofPopMatrix();

    ofPushMatrix();
    ofSetHexColor(0xFF0000);
    ofTranslate(20 + imSize + 20 + imDiag + 20, 20 + imSize + 20 + imDiag);
    rightSpectrum.draw();
    ofDrawLine(0, 0, imSize / 2, 0);
    ofPopMatrix();
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
