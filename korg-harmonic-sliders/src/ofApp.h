#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

struct Sound {
    unsigned int pitch;
    float velocity;
    uint64_t started;
    uint64_t died;
};

struct LoopSound {
    Sound sound;
    uint64_t at;
};

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
        void handleKnob(int slider, int value);
        void handleSolo(int slider);
        void handleMute(int slider);
        void handleButtons(int button, int value);
        void audioOut(ofSoundBuffer &outBuffer);

        ofxMidiIn midiIn;
        ofxMidiMessage midiMessage;
    
        vector <float> freqs;
        vector <Sound> notes;

        vector <float> harmonicAmps;
        float attack;
        float sustain;
        float release;
    
        bool recording;
        bool loop_set;
        uint64_t loop_start;
        uint64_t loop_length;
        bool playing;

        vector <LoopSound> loop_sounds;

        uint64_t step;
};
