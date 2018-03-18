#include "ofApp.h"

int min(int a, int b) { return a < b ? a : b; }
int max(int a, int b) { return a > b ? a : b; }
float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }

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
    fftLeftIn = new kiss_fft_cpx[imSize];
    fftLeftOut = new kiss_fft_cpx[imSize];
    fftRightIn = new kiss_fft_cpx[imSize];
    fftRightOut = new kiss_fft_cpx[imSize];

    for (size_t i = 0; i < imSize * imSize; i++) {
        fftGreyIn[i].i = 0.0;  // imaginary part is always zero
    }

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
        
        for (size_t x = 0; x < imSize; x++) {
            for (size_t y = 0; y < imSize; y++) {
                ofColor c = pixels.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
                float l = c.getLightness();
                greyPixels.setColor(x, y, ofColor(l));
                fftGreyIn[y * imSize + x].r = l;
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
                if (ring < imSize / 2) {
                    size_t outI = y * imSize + x;
                    kiss_fft_cpx out = fftOut[outI];

                    float l = log(sqrt(pow(out.r, 2) + pow(out.i, 2)) / 255) * 16;
                    fftPixels.setColor(x, y, ofColor(max(0.0, min(l, 255.0))));

                    double theta = asin(y / (ring ? ring : 1));
                    double pan = theta / (PI / 2);  // quarter turn

                    double real = out.r / (ring * PI / 2);
                    double imaj = out.i / (ring * PI / 2);

                    fftLeftIn[ring].r += (1 - pan) * real;
                    fftLeftIn[ring].i += (1 - pan) * imaj;
                    fftLeftIn[imSize - ring].r += (1 - pan) * real;
                    fftLeftIn[imSize - ring].i = (1 - pan) * imaj;

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
    vidGrabber.draw(20, 20, camWidth, camHeight);
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
        float left = log(sqrt(pow(fftLeftIn[i].r, 2) + pow(fftLeftIn[i].i, 2))) * 3;
        leftSpectrum.addVertex(ofPoint(i, -left));
        float right = log(sqrt(pow(fftRightIn[i].r, 2) + pow(fftRightIn[i].i, 2))) * 3;
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
