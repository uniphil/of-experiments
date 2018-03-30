//
//  sound.cpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#include "sound.hpp"

Sound::Sound(unsigned int p, double vel, uint64_t now) {
    alive = true;
    frequency = pow(2, ((int)p - 69) / 12.0) * 440;
    velocity = vel;
    on = now;
}

void Sound::repitch(unsigned int to) {
    frequency = pow(2, ((int)to - 69) / 12.0) * 440;
}

float Sound::getSample(uint64_t at, kiss_fft_cpx * frequencies, unsigned int N) {
    double dt = (at - on) / 1000000.0;
    double a = 0;
    for (int harmonic = 0; harmonic < HARMONICS; harmonic++) {
        double f = frequency * harmonic;
        int bin = round(ofMap(f, 0, 20000, 0, N/2));
        double h = abs(frequencies[bin]);
        a += h * sin(dt * TWO_PI * f) / (1 + harmonic);
    }
    a /= HARMONICS;
    return a;
//    return sin(dt * TWO_PI * frequency);
}

void Sound::release(uint64_t now) {
    alive = false;
    off = now;
}
