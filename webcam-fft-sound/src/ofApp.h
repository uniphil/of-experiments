#pragma once

#include "ofMain.h"
#include "kiss_fft.h"
#include "kiss-ndfft.hpp"

#define N_FFT_OUT 2048

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

        unsigned int imSize;
        unsigned int camWidth;
        unsigned int camHeight;
        ofVideoGrabber vidGrabber;
        ofPixels greyPixels;
        ofTexture greyTexture;
        ofPixels fftPixels;
        ofTexture fftTexture;
    
        kiss_fftnd_cfg kissNdCfg;
        kiss_fft_cpx * fftGreyIn;
        kiss_fft_cpx * fftOut;

        kiss_fft_cfg kissCfg;
        kiss_fft_cpx * fftLeftIn;
        kiss_fft_cpx * fftLeftOut;
        kiss_fft_cpx * fftRightIn;
        kiss_fft_cpx * fftRightOut;
    
        uint64_t audioStep;

        bool ready;
};
