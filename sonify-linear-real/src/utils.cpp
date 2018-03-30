//
//  utils.cpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#include "utils.hpp"

double abs(kiss_fft_cpx p) {
    return sqrt(pow(p.r, 2) + pow(p.i, 2));
}
