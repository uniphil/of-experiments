#pragma once

#include "ofMain.h"
#include "kiss_fft.h"
#include "hann.hpp"

#define F 44100
#define W 1280
#define H 720
#define N 512
#define P 512
#define RP 8

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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

    ofVideoGrabber source;
    ofImage framed;

    Hann * window;

    kiss_fft_cfg forward;
    kiss_fft_cpx * linearized;
    kiss_fft_cpx * frequencies;

    double theta;
};
