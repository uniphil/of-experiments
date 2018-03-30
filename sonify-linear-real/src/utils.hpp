//
//  utils.hpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#ifndef utils_hpp
#define utils_hpp

#include "kiss_fft.h"

#define F 44100
#define W 1280
#define H 720
#define N 512
#define P 512
#define RP 4
#define ATTACK 0.05
#define DECAY 0.1
#define RELEASE 0.5
#define HARMONICS 8

double abs(kiss_fft_cpx p);

int keyToPitch(int key);

#endif /* utils_hpp */
