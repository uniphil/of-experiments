#pragma once

#include "ofMain.h"
#include "plane.hpp"
#include "projectile.hpp"

#define GRAVITY 9.81  // metres / sec^2
#define PLANE_MAX_SPEED 130 // m/s
#define PLANE_CRUISE_SPEED 83  // m/s
#define PLANE_MIN_SPEED PLANE_CRUISE_SPEED - (PLANE_MAX_SPEED - PLANE_CRUISE_SPEED)  // m/s
#define VIEW_HEIGHT 3000  // metres... predictor could hit up to 4800 or something though
#define MAX_SCOPE_TRACK 0.873  // radian / sec (50deg/sec)
#define LAUNCH_VELOCITY 881  // metres / sec (bofors 40 L60)
#define RELOAD_TIME 0.7  // seconds, 80-100 rounds/min
#define BORINGNESS 0.05  // 0.01 is very boring

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
    
        // "aim": where the launcher is pointing
        double aimHeight;
        double aimTheta;

        // "view": where the scope is pointing (laggs behind mouse due to max on rotational velocity)
        double lastViewPosition;
        double viewVelocity;
        double viewTheta;

        vector <Plane> planes;
        vector <Projectile> projectiles;
};
