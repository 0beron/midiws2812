//#include "WS2812.hpp"
#include <algorithm>
#include "particle.h"
#include "render.h"
#include <memory>
#include "global.h"
#include "FastLED.h"

void drawParticle(CRGB *leds, Particle p) {
    if (p.y > 0) {
        int s,e, bri;

        int f = RES/glb_maxleds;
        if (p.fall < 0) {
            s = (p.x/f);
            e = (p.x/f)+1;
        } else {
            s = ((p.x - (p.y/p.fall))/f)-2;
            e = ((p.x + (p.y/p.fall))/f)+2;
        }
    
        for (int n=s; n<e; n++) {
            CRGB col;
            if (n < 0) continue;
            if (n >= glb_maxleds) continue;
            int d1 = p.x-(n*f);
            int d = std::abs(d1);
            if (p.fall < 0) {
                bri = p.y;
            } else {
                bri = p.y-(p.fall*d);
            }
            bri = std::max(std::min(bri/(RES/255),255),0);
            col = p.col;
            nscale8(&col, 1, bri);
            leds[n] |= col;
            //leds->maxPixelColor(n, RGB_bri(p.col, bri));
        }
        
    }   
}