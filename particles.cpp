#include "particles.h"
#include <algorithm>
//#include "WS2812.hpp"
#include "hardware/regs/rosc.h"
#include "hardware/regs/addressmap.h"
#include "particle.h"
#include "render.h"
#include "FastLED.h"

#include "global.h"

// Von Neumann extractor: From the input stream, his extractor took bits, two at a time (first and second, then third and fourth, and so on). If the two bits matched, no output was generated. If the bits differed, the value of the first bit was output. 



uint32_t rnd_whitened(void){
    int k, random=0;
    int random_bit1, random_bit2;
    volatile uint32_t *rnd_reg=(uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
    
    for(k=0;k<32;k++){
        while(1){
            random_bit1=0x00000001 & (*rnd_reg);
            random_bit2=0x00000001 & (*rnd_reg);
            if(random_bit1!=random_bit2) break;
        }

	random = random << 1;        
        random=random + random_bit1;

    }
    return random;
}

int approximate_pow(int base, double exp) {
    // Approximate pow using integer arithmetic
    // We will use an iterative method to approximate the power
    int result = 1;
    int fixed_point_base = base << 16; // Scale base to fixed-point format

    for (int i = 0; i < 16; ++i) {
        fixed_point_base = fixed_point_base * base >> 8;
        result = result * fixed_point_base >> 16;
    }
    
    return result >> 8;
}

int rnd(int min, int max) {
    uint32_t range = max-min;

    return (rnd_whitened() % range) + min;
}

void Particles::handleNoteOn(int channel, int note, int velocity) {

}

void Particles::handleNoteOff(int channel, int note, int velocity) {
}

void Particles::handleFrameUpdate() {
    int wnew = rnd(0, 1000);
    if (wnew > 970) {
        for (int i=0; i<10; i++) {
            if (p[i].y <= 0) {
                uint8_t r = rnd(0,255);
                uint8_t g = rnd(0,255);
                uint8_t b = rnd(0,255);
                CRGB cc = CRGB(r,g,b);

                p[i] = Particle(rnd(0,RES), 1, rnd(-RES/2500, RES/2500), rnd(200,RES/200), cc);
                break;
            }
        }
    }

    FastLED.clear();

    for (int i=0; i<10; i++) {

        if (p[i].y > 0) {
            p[i].x += p[i].xvel;
            p[i].y += p[i].yvel;
            p[i].yvel -= 1;


            drawParticle(leds, p[i]);
            
         
        }   

    }

}
