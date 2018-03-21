#pragma once

#include "ofMain.h"
#include "kiss_fft.h"

#define N 4096
#define R 44100
#define C 2
#define HIGHEST 4186.01  // highest note on 88-key piano

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void audioOut(ofSoundBuffer &outBuffer);

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

        kiss_fft_cfg kissForward;
        kiss_fft_cfg kissInverse;
        kiss_fft_cpx * lastTimeSamples;
        kiss_fft_cpx * timeSamples;
        kiss_fft_cpx * frequencyBins;
    
        float max;
};
