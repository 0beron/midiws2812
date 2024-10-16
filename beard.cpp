#include "beard.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"


Beard::Beard(CRGB *leds)
        : Effect(leds) {
    

    for(int i=0; i<33; i++) {
        uvs[i].u = (i*0.2f)/33.0;
        uvs[i].v = (i*0.5f)/33.0;
    }
    for(int i=33; i<51; i++) {
        uvs[i].u = 0.2f+(((i-33)*0.2f)/18.0);
        uvs[i].v = 0.5f-(((i-33)*0.3f)/18.0);
    }
    for(int i=51; i<93; i++) {
        uvs[i].u = 0.4f+(((i-51)*0.1f)/42.0);
        uvs[i].v = 0.2f+(((i-51)*0.8f)/42.0);
    }
    for(int i=93; i<137; i++) {
        uvs[i].u = 0.5f+(((i-93)*0.1f)/44.0);
        uvs[i].v = 1.0f-(((i-93)*0.8f)/44.0);
    }
    for(int i=137; i<158; i++) {
        uvs[i].u = 0.6f+(((i-137)*0.2f)/21.0);
        uvs[i].v = 0.2f+(((i-137)*0.3f)/21.0);
    }
    for(int i=158; i<180; i++) {
        uvs[i].u = 0.8f+(((i-158)*0.2f)/22.0);
        uvs[i].v = 0.5f-(((i-158)*0.5f)/22.0);
    }
    for(int i=0; i<180; i++) {
        printf("IUV: %d %f %f\n", i, uvs[i].u, uvs[i].v);
    }
}



int Beard::nLEDS() {
    return 180;
}

void Beard::handleNoteOn(int channel, int note, int velocity) {

}

void Beard::handleNoteOff(int channel, int note, int velocity) {
   
}

void Beard::handlePolyAT(int channel, int note, int value) {
   
}

void  Beard::handleFrameUpdate() {
    
}