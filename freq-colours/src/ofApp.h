#pragma once

#include "ofMain.h"
#include "kiss_fftnd.h"

#define W 640
#define H 480

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

    ofVideoGrabber camera;
    size_t imSize;
    ofImage windowed;
    ofImage result;

    kiss_fftnd_cfg forward;
    kiss_fftnd_cfg inverse;
    kiss_fft_cpx * grey;
    kiss_fft_cpx * freqs;
    kiss_fft_cpx * freqsOut;
    kiss_fft_cpx * freqsR;
    kiss_fft_cpx * freqsG;
    kiss_fft_cpx * freqsB;
    kiss_fft_cpx * outR;
    kiss_fft_cpx * outG;
    kiss_fft_cpx * outB;
};
