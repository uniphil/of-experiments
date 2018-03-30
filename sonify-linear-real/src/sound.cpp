//
//  sound.cpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#include "sound.hpp"

Sound::Sound(unsigned int pitch, double velocity, uint64_t now) {
    alive = true;
    this->pitch = pitch;
    frequency = pow(2, ((int)pitch - 69) / 12.0) * 440;
    this->velocity = velocity;
    on = now;
}

float Sound::getSample(uint64_t at, kiss_fft_cpx * frequencies, unsigned int n) {
    double dt = (at - on) / 1000000.0;
    double a = 0;
    double envelope = 1.0;
    if (dt < ATTACK) {
        envelope = ofMap(dt, 0, ATTACK, 0, 1);
    }
    if (!alive) {
        envelope *= ofMap(dt, (at - off) / 1000000.0, (at - off / 1000000.0) + RELEASE, 1, 0, true);
    }
    for (int harmonic = 0; harmonic < HARMONICS; harmonic++) {
        double f = frequency * harmonic;
        int bin = round(ofMap(f, 0, 20000, 0, n/2));
        double h = abs(frequencies[bin]);
        a += h * sin(dt * TWO_PI * f) / (1 + harmonic);
    }
    a /= HARMONICS;
    return a * envelope;
}

bool Sound::active() {
    return alive;
}

void Sound::release(uint64_t now) {
    alive = false;
    off = now;
}
