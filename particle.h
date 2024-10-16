#ifndef PARTICLE_H
#define PARTICLE_H

#include "FastLED.h"

#define RES 60000
#define FALL 4


class Particle {
public:
    int x,y,xvel,yvel,dir,fall;
    CRGB col;
    Particle() {
        x = 0;
        y = 0;
        xvel = 0;
        yvel = 0;
        col=0;
        dir=0;
        fall=4;
    }

    Particle(int x, int y, int xvel, int yvel, CRGB col, int fall)
        : x(x), y(y), xvel(xvel), yvel(yvel), col(col), dir(0), fall(fall) {

    }
};


#endif