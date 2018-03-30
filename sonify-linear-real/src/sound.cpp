//
//  sound.cpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#include "sound.hpp"

Sound::Sound(unsigned int pitch, double velocity, uint64_t now) {
    this->alive = true;
    this->pitch = pitch;
    this->frequency = pow(2, ((int)pitch - 69) / 12.0) * 440;
    this->velocity = velocity;
    this->on = now;
}

float Sound::getSample(uint64_t at, kiss_fft_cpx * frequencies, unsigned int n) {
    double dt = (at - on) / (double)F;
    double a = 0;
    double envelope = 1.0;
    if (dt < ATTACK) {
        envelope *= ofMap(dt, 0, ATTACK, 0, 1, true);
    } else {
        envelope *= 1 / (1 + pow(dt, 2) * pow(DECAY * 10, 2));
    }
    if (!alive) {
        envelope *= ofMap(at, off, off + RELEASE * F, 1, 0, true);
    }
    for (int harmonic = 0; harmonic < HARMONICS; harmonic++) {
        double f = frequency * harmonic;
        int bin = round(ofMap(f, 0, 20000, 0, n/2));
        double h = abs(frequencies[bin]);
        a += h * sin(dt * TWO_PI * f) / (1 + harmonic);
    }
    a /= HARMONICS * 3;
    return a * envelope;
}

bool Sound::active() {
    return alive;
}

bool Sound::dead(uint64_t when) {
    return !alive &&
           when > off + RELEASE * F;
}

void Sound::release(uint64_t now) {
    alive = false;
    off = now;
}
