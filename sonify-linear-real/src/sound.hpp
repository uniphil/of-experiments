//
//  sound.hpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#ifndef sound_hpp
#define sound_hpp

#include "ofMain.h"
#include "kiss_fft.h"
#include "utils.hpp"

class Sound {
public:
    Sound(unsigned int pitch, double velocity, uint64_t on);
    float getSample(uint64_t at, kiss_fft_cpx * frequencies, unsigned int n);
    bool active();
    bool dead(uint64_t when);
    void release(uint64_t now);
    unsigned int pitch;
private:
    double frequency;
    double velocity;
    uint64_t on;
    bool alive;
    uint64_t off;
};

#endif /* sound_hpp */
