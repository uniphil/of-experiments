#pragma once

#include "ofMain.h"
#include "kiss_fft.h"
#include "kiss_fftnd.h"

#define F 44100
#define S 512

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void audioOut(ofSoundBuffer &outBuffer);
        void processFrame(ofPixels & imagePixels);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    float last;

    ofVideoGrabber grabber;
    unsigned int imSize;

    kiss_fftnd_cfg webcamToFreq;
    kiss_fft_cpx * image;
    kiss_fft_cpx * imFreqs;

    ofImage masked;
    ofImage transformed;

    double angle;
    double * linFreqAmps;
    double * linFreqPhases;
};
