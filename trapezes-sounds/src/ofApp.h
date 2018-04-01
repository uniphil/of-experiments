#pragma once

#include "ofMain.h"
#define ATTACK 0.1
#define DECAY 0.1
#define SUSTAIN 0.3
#define RELEASE 0.6

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

        void audioOut(ofSoundBuffer &outBuffer);
  
        float stability;
        uint64_t outStep;
        vector <float> audio;
        bool hasDrawn;
};
