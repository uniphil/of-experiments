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

int keyToPitch(int key) {
    if      (key == 'a') return 49;
    else if (key == 'w') return 50;
    else if (key == 's') return 51;
    else if (key == 'e') return 52;
    else if (key == 'd') return 53;
    else if (key == 'r') return 54;
    else if (key == 'f') return 55;
    else if (key == 'g') return 56;
    else if (key == 'y') return 57;
    else if (key == 'h') return 58;
    else if (key == 'u') return 59;
    else if (key == 'j') return 60;
    else if (key == 'k') return 61;
    else if (key == 'o') return 62;
    else if (key == 'l') return 63;
    else if (key == 'p') return 64;
    else if (key == ';') return 65;
    else if (key == '[') return 66;
    else if (key == '\'') return 67;
    return -1;
}
