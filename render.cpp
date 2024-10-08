//#include "WS2812.hpp"
#include <algorithm>
#include "particle.h"
#include "render.h"
#include <memory>
#include "global.h"
#include "FastLED.h"

void drawParticle(CRGB *leds, Particle p) {
    if (p.y > 0) {
        
        int f = RES/LED_LENGTH;
        int s = (p.x - (p.y/FALL))/f;
        int e = (p.x + (p.y/FALL))/f;
    
        for (int n=s-2; n<=e+2; n++) {
            CRGB col;
            if (n < 0) continue;
            if (n >= LED_LENGTH) continue;
            int d1 = p.x-(n*f);
            int d = std::abs(d1);
            int bri = p.y-(FALL*d);
            bri = std::max(std::min(bri/(RES/255),255),0);
            col = p.col;
            nscale8(&col, 1, bri);
            leds[n] |= col;
            //leds->maxPixelColor(n, RGB_bri(p.col, bri));
        }
        
    }   
}