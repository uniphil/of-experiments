#pragma once

#include "ofMain.h"

#define GRAVITY 9.81  // metres / sec^2
#define VIEW_HEIGHT 3000  // metres
#define MAX_SCOPE_TRACK 1  // radian / sec
#define LAUNCH_VELOCITY 800  // metres / sec (??)


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

        ofPoint mouseAim;
        uint64_t lastUpdate;
        double aimHeight;
        double viewTheta;
        double aimTheta;
};
