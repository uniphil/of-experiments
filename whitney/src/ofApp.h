#pragma once

#include "ofMain.h"
#include "plane.hpp"
#include "projectile.hpp"

#define GRAVITY 9.81  // metres / sec^2
#define PLANE_MAX_SPEED 130 // m/s
#define PLANE_CRUISE_SPEED 83  // m/s
#define PLANE_MIN_SPEED PLANE_CRUISE_SPEED - (PLANE_MAX_SPEED - PLANE_CRUISE_SPEED)  // m/s
#define VIEW_HEIGHT 4500  // metres
#define MAX_SCOPE_TRACK 1  // radian / sec
#define LAUNCH_VELOCITY 881  // metres / sec (boffors 40 L60)
#define RELOAD_TIME 1.0  // seconds, 30s for boffors 40 L60
#define BORINGNESS 0.5  // 0.1 is boring

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
