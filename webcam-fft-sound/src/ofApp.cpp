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
    shiftedPixels.allocate(imSize, imSize, OF_PIXELS_RGB);
    shiftedTexture.allocate(shiftedPixels);
    fftPixels.allocate(imSize, imSize, OF_PIXELS_RGB);  // mono?
    fftTexture.allocate(fftPixels);
    
    int dims [2] = { (int)imSize, (int)imSize };
    kissCfg = kiss_fftnd_alloc(dims, 2, false, 0, 0);
    fftGreyIn = new kiss_fft_cpx[imSize * imSize];
    fftOut = new kiss_fft_cpx[imSize * imSize];
    for (size_t i = 0; i < imSize * imSize; i++) {
        fftGreyIn[i].i = 0.0;  // imaginary part is always zero
    }
    
    rowAverage.assign(imSize, 0.0);
    colAverage.assign(imSize, 0.0);
    squareAverage.assign(imSize / 2, 0.0);
    ringAverage.assign(imSize / 2, 0.0);

    ofSetVerticalSync(true);
    ready = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    vidGrabber.update();

    if (vidGrabber.isFrameNew() && ready) {  // boo races
        ofPixels & pixels = vidGrabber.getPixels();
        pixels.mirror(false, true);
        
        for (size_t x = 0; x < imSize; x++) {
            for (size_t y = 0; y < imSize; y++) {
                ofColor c = pixels.getColor(x + (camWidth - imSize) / 2, y + (camHeight - imSize) / 2);
                float l = c.getLightness();
                greyPixels.setColor(x, y, ofColor(l));
                float shifted = l * pow(-1, x+y);
                shiftedPixels.setColor(x, y, ofColor(ofMap(shifted, -255, 255, 0, 255)));
                fftGreyIn[y * imSize + x].r = shifted;  // shifted
            }
        }

        greyTexture.loadData(greyPixels);
        shiftedTexture.loadData(shiftedPixels);
        kiss_fftnd(kissCfg, fftGreyIn, fftOut);
        
        for (int i = 0; i < rowAverage.size(); i++) rowAverage[i] = 0;
        for (int i = 0; i < colAverage.size(); i++) colAverage[i] = 0;
        for (int i = 0; i < squareAverage.size(); i++) squareAverage[i] = 0;
        for (int i = 0; i < ringAverage.size(); i++) ringAverage[i] = 0;

        for (size_t x = 0; x < imSize; x++) {
            for (size_t y = 0; y < imSize; y++) {
                size_t outI = y * imSize + x;
                float l = log(sqrt(pow(fftOut[outI].r, 2) + pow(fftOut[outI].i, 2))) * 8;
                fftPixels.setColor(x, y, ofColor(max(0.0, min(l, 255.0))));

                rowAverage[y] += l / imSize;
                colAverage[x] += l / imSize;
                int xc = abs((int)imSize / 2 - (int)x);
                int yc = abs((int)imSize / 2 - (int)y);
                if (xc < squareAverage.size() && yc < squareAverage.size()) {
                    int currentSquare = max(xc, yc);
                    squareAverage[currentSquare] += l / (currentSquare * 2);
                }
                int currentRing = sqrt(pow(xc, 2) + pow(yc, 2));
                if (currentRing < ringAverage.size()) {
                    ringAverage[currentRing] += l / (2 * PI * currentRing);
                }
            }
        }

        fftTexture.loadData(fftPixels);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(0xFF);
    vidGrabber.draw(20, 20, camWidth, camHeight);
    greyTexture.draw(20 + 20 + camWidth, 20, imSize, imSize);
    shiftedTexture.draw(20, 20 + 20 + imSize, imSize, imSize);
    fftTexture.draw(20 + 20 + imSize, 20 + 20 + imSize, imSize, imSize);

    float graphMax = 100;

    ofPushMatrix();
    ofTranslate(20, (20 + imSize * 2) + 100);
    ofPolyline colGraph;
    for (int i = 0; i < colAverage.size(); i++) {
        colGraph.addVertex(ofPoint(i, graphMax - colAverage[i]));
    }
    colGraph.draw();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(20 + 20 + imSize, (20 + imSize) * 2 + 100);
    ofPolyline rowGraph;
    for (int i = 0; i < rowAverage.size(); i++) {
        rowGraph.addVertex(ofPoint(i, graphMax - rowAverage[i]));
    }
    rowGraph.draw();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(20 + 20 + imSize + 20 + imSize, (20 + imSize) * 2 + 100);
    ofPolyline squareGraph;
    for (int i = 0; i < squareAverage.size(); i++) {
        squareGraph.addVertex(ofPoint(i, graphMax - squareAverage[i]));
    }
    squareGraph.draw();
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(20 + 20 + imSize + 20 + imSize + 20 + squareAverage.size(), (20 + imSize) * 2 + 100);
    ofPolyline ringGraph;
    for (int i = 0; i < ringAverage.size(); i++) {
        ringGraph.addVertex(ofPoint(i, graphMax - ringAverage[i]));
    }
    ringGraph.draw();
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
