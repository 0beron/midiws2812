#include "streb.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"
#include "pico/stdlib.h"

Streb::Streb(CRGB *leds)
        : Effect(leds) {
    for(int i=0; i<60; i++) {
        particles[i] = Particle(0,0,0,0,0);
        ledbuff[i] = CRGB::Black;
    }
}

int Streb::nLEDS() {
    return 32;
}

void Streb::handleNoteOn(int channel, int note, int velocity) {
    int off;
    if (note >= 50 && note < 110 && channel > 0 && channel < 3) {
        if (channel == 1) {
            off = settings[CHORD_OFFSET];
        } else {
            off = settings[BASS_OFFSET];
        }
        int mnote = (109+off)-note;
        printf("Streb note on, offset is %d", off);
        if (mnote > 59) {
            mnote = 59;
        }
        if (mnote < 0) {
            mnote = 0;
        }
        Particle *p = &particles[mnote];
        p->x = ((RES/LED_LENGTH)*(mnote));
        p->y = 1;
        p->yvel = 0;
        if (channel == 0) {
            p->col = CRGB(255,0,255);
        } else if (channel == 1) {
            p->col = CRGB(0,255,255);
        } else {
            p->col = CRGB(255,0,255);
        }
    }

    
    
    if (channel == 0) {
        running++;
        lastnote = note;
    }

}

void Streb::handleNoteOff(int channel, int note, int velocity) {
     int off;
     if (note > 50 && note < 110 && channel > 0 && channel < 3) {
        //leds[note-30] = CRGB(0, 0, 0);
        if (channel == 1) {
            off = settings[CHORD_OFFSET];
        } else {
            off = settings[BASS_OFFSET];
        }        
        int mnote = (109+off)-note;
        if (mnote > 59) {
            mnote = 59;
        }
        if (mnote < 0) {
            mnote = 0;
        }
     
        particles[mnote].yvel = -2;
    }
    if (channel == 0) {
        running--;
    }
}


void Streb::handleCC(int channel, int cc, int value) {
    CRGB col;
    if (cc == 11 || cc == 2) {
        lastPrs = value;
        int v = value / 2;
        
        for (int i=0; i<60; i++) {

            if (particles[i].y > 0 && particles[i].yvel == 0) {
                particles[i].y = (v * RES/2)/128;
            }   
        }
    }
}

void Streb::handleFrameUpdate() {
    FastLED.clear();

    for (int i=0; i<60; i++) {

        if (particles[i].y > 0) {
            particles[i].y += particles[i].yvel;
            if (particles[i].yvel != 0) {
                particles[i].yvel -= 1;
            }

            drawParticle(leds, particles[i]);
        }   
    }


    framediv++;
    if (framediv > 2) {    
        for(int i=15; i>0; i--){
            ledbuff[i] = ledbuff[i-1];
        }
        if (running > 0) {
            ledbuff[0] = CHSV((lastnote%12)*21,255, lastPrs*2);
        } else {
            ledbuff[0] = CRGB::Black;
        }
        
        framediv=0;

    }

    for(int i=0; i<16; i++) {
        leds[i] = ledbuff[i];
    }

}

