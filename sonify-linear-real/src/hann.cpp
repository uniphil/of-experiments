//
//  hann.cpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#include "hann.hpp"

Hann::Hann(unsigned int n) {
    this->n = n;
    window = new double[n];
    for (int i = 0; i < n; i++) {
        window[i] = 0.5 * (1 - cos(TWO_PI * i / (n - 1)));
    }
}

void Hann::apply(double * samples) {
    for (int i = 0; i < n; i++) {
        samples[i] *= window[i];
    }
}

void Hann::apply(kiss_fft_cpx * samples) {
    apply(samples, false);
}

void Hann::apply(kiss_fft_cpx * samples, bool scaleReal) {
    for (int i = 0; i < n; i++) {
        samples[i].r *= window[i];
        scaleReal && (samples[i].i *= window[i]);
    }
}
