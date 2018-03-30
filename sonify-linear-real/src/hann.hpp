//
//  hann.hpp
//  sonify-linear-real
//
//  Created by phil on 2018-03-29.
//

#ifndef hann_hpp
#define hann_hpp

#include <math.h>

#define TWO_PI 6.283185307179586476925286766559005768394338798750211641949

class Hann {
private:
    double * window;
    unsigned int n;
public:
    Hann(unsigned int N);
    void apply(double * samples);
};

#endif /* hann_hpp */
