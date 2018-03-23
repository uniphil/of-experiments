#pragma once

#include "ofMain.h"
#include "kiss_fftnd.h"

typedef struct {
    ofPixels pix;
    ofTexture tex;
} im;

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

    int camWidth;
    int camHeight;
    int imSize;

    ofVideoGrabber vidGrabber;

    im grey;
    im noise;
    im out;
    
    float * staticNoise;

    kiss_fftnd_cfg forwardCfg;
    kiss_fftnd_cfg inverseCfg;
    kiss_fft_cpx * greySamples;
    kiss_fft_cpx * greyFreqs;
    kiss_fft_cpx * noiseSamples;
    kiss_fft_cpx * noiseFreqs;
    kiss_fft_cpx * outSamples;

    bool ready;
};
