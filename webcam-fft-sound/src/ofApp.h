#pragma once

#include "ofMain.h"
#include "kiss-ndfft.hpp"

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

        unsigned int camWidth;
        unsigned int camHeight;
        ofVideoGrabber vidGrabber;
        ofPixels greyPixels;
        ofTexture greyTexture;
        ofPixels shiftedPixels;
        ofTexture shiftedTexture;
        ofPixels fftPixels;
        ofTexture fftTexture;
    
        kiss_fftnd_cfg kissCfg;
        kiss_fft_cpx * fftGreyIn;
        kiss_fft_cpx * fftOut;

        bool ready;
};
