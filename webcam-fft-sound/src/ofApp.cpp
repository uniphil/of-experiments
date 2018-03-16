#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ready = false;

    camWidth = 320;
    camHeight = 240;

    vidGrabber.setDeviceID(0);  // fingers crossed
    vidGrabber.setDesiredFrameRate(60);
    vidGrabber.initGrabber(camWidth, camHeight);

    greyPixels.allocate(camWidth, camHeight, OF_PIXELS_RGB);
    greyTexture.allocate(greyPixels);
    shiftedPixels.allocate(camWidth, camHeight, OF_PIXELS_RGB);
    shiftedTexture.allocate(shiftedPixels);
    fftPixels.allocate(camWidth, camHeight, OF_PIXELS_RGB);  // mono?
    fftTexture.allocate(fftPixels);
    
    int dims [2] = { (int)camHeight, (int)camWidth };
    kissCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    fftGreyIn = new kiss_fft_cpx[camWidth * camHeight];
    fftOut = new kiss_fft_cpx[camWidth * camHeight];
    for (size_t i = 0; i < camWidth * camHeight; i++) {
        fftGreyIn[i].i = 0.0;  // imaginary part is always zero
    }

    ofSetVerticalSync(true);
    ready = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();

    if (vidGrabber.isFrameNew() && ready) {  // boo races
        ofPixels & pixels = vidGrabber.getPixels();
        
        for (size_t x = 0; x < camWidth; x++) {
            for (size_t y = 0; y < camHeight; y++) {
                ofColor c = pixels.getColor(x, y);
                float l = c.getLightness();
                greyPixels.setColor(x, y, ofColor(l));
                float shifted = l * pow(-1, x+y);
                shiftedPixels.setColor(x, y, ofColor(ofMap(shifted, -255, 255, 0, 255)));
                fftGreyIn[y * camWidth + x].r = shifted / 255;  // shifted
            }
        }

        greyTexture.loadData(greyPixels);
        shiftedTexture.loadData(shiftedPixels);
        kiss_fftnd(kissCfg, fftGreyIn, fftOut);

        for (size_t x = 0; x < camWidth; x++) {
            for (size_t y = 0; y < camHeight; y++) {
                size_t outI = y * camWidth + x;
                float l = sqrt(pow(fftOut[outI].r, 2) + pow(fftOut[outI].i, 2));
                fftPixels.setColor(x, y, ofColor(l));
            }
        }

        fftTexture.loadData(fftPixels);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0xFF);
    vidGrabber.draw(20, 20, camWidth, camHeight);
    greyTexture.draw(20 + 20 + camWidth, 20, camWidth, camHeight);
    shiftedTexture.draw(20, 20 + 20 + camHeight, camWidth, camHeight);
    fftTexture.draw(20 + 20 + camWidth, 20 + 20 + camHeight, camWidth, camHeight);
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
