#pragma once

#include "ofMain.h"
#include "ofxMidi.h"
#include "kiss_fft.h"
#include "utils.hpp"
#include "hann.hpp"
#include "sound.hpp"

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		void setup();
		void update();
		void draw();
        void audioOut(ofSoundBuffer &outBuffer);
        void newMidiMessage(ofxMidiMessage& eventArgs);

		void keyPressed(int key);
		void keyReleased(int key);
    
        void noteOn(unsigned int pitch, double velocity);
        void noteOff(unsigned int pitch);

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

    ofxMidiIn midiIn;

    uint64_t audioFrame;

    vector <Sound> notes;
};
