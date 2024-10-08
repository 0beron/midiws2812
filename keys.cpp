#include "keys.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"

Keys::Keys(CRGB *leds)
        : Effect(leds) {
    for(int i=0; i<60; i++) {
        particles[i] = Particle(0,0,0,0,0);
    }
}

void Keys::handleNoteOn(int channel, int note, int velocity) {
    if (note > 30 && note < 90) {
        //leds[note-30] = CRGB(0, 0, 50);
        Particle *p = &particles[note-30];
        p->x = (RES/LED_LENGTH)*(note-30);
        p->y = RES/4;
        p->col = CRGB(0,0,255);
    }
}

void Keys::handleNoteOff(int channel, int note, int velocity) {
    if (note > 30 && note < 90) {
        //leds[note-30] = CRGB(0, 0, 0);
        particles[note-30].y = 0;
    }
}

void Keys::handlePolyAT(int channel, int note, int value) {
    if (note > 30 && note < 90) {
        if (particles[note-30].y > 0) {
            particles[note-30].y = (RES/4) + (value * ((3*RES/4) / 127));
            particles[note-30].col = CRGB(value*2, value*2, std::max(value*2, 255-(value*2)));
            //leds->setPixelColor(note-30, value/3, 0, 50-(value/3));
        }
    }
}

void  Keys::handleFrameUpdate() {
    FastLED.clear();
    for(int i=0;i<60;i++) {
        drawParticle(leds,particles[i]);
    }
    
}
