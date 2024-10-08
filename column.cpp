#include "column.h"
#include <algorithm>
#include "WS2812.hpp"
#include "particle.h"
#include "render.h"

Column::Column(std::shared_ptr<WS2812> leds)
        : Effect(leds) {
    for(int i=0; i<60; i++) {
        particles[i] = Particle(0,0,0,0,0);

    }
}

void Column::handleNoteOn(int channel, int note, int velocity) {
    if (note > 30 && note < 90) {
        //leds->setPixelColor(note-30, 0, 0, 50);
        Particle *p = &particles[note-30];
        p->x = (RES/leds->length)*(note-30);
        p->y = RES/4;
        p->col = WS2812::RGB(0,0,255);
    }
}

void Column::handleNoteOff(int channel, int note, int velocity) {
    if (note > 30 && note < 90) {
        particles[note-30].y = 0;
    }
}

void Column::handlePolyAT(int channel, int note, int value) {
    if (note > 30 && note < 90) {
        if (particles[note-30].y > 0) {
            particles[note-30].y = (RES/4) + (value * ((3*RES/4) / 127));
            particles[note-30].col = WS2812::RGB(value*2, value*2, std::max(value*2, 255-(value*2)));
            //leds->setPixelColor(note-30, value/3, 0, 50-(value/3));
        }
    }
}

void Column::handleFrameUpdate() {
    leds->fill(0);

    for(int i=0;i<60;i++) {

    }

    for(int i=0;i<60;i++) {
        drawParticle(leds,particles[i]);
    }
    
}
