#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

class ofApp : public ofBaseApp, public ofxMidiListener {

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
		
        void newMidiMessage(ofxMidiMessage& eventArgs);
        void handleNoteOff(int pitch);
        void handleNoteOn(int pitch, int velocity);
        void handleSlider(int slider, int value);
        void handleSolo(int slider);
        void handleMute(int slider);
        void audioOut(ofSoundBuffer &outBuffer);

        ofxMidiIn midiIn;
        ofxMidiMessage midiMessage;
    
        vector <float> freqs;
    
        float amp;
        float freq;
        unsigned int keys_down_count;
        vector <float> harmonicAmps;
    
        uint64_t step;
};
