#include "pianoroll.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"

Pianoroll::Pianoroll(CRGB *leds)
        : Effect(leds) {
}


void Pianoroll::handleNoteOn(int channel, int note, int velocity) {
    running++;
    lastnote = note;
}

void Pianoroll::handleNoteOff(int channel, int note, int velocity) {
    running--;
}

void Pianoroll::handleCC(int channel, int cc, int value) {
    if (cc == 2) {
        lastPrs = value;
    }
}

void Pianoroll::handleFrameUpdate() {
    framediv++;
    if (framediv > 2) {    
        for(int i=0; i<(LED_LENGTH/2); i++){
            leds[i] = leds[i+1];
        }
        if (running > 0) {
            leds[(LED_LENGTH/2)-1] = CHSV((lastnote%12)*21,255, lastPrs*2);
        } else {
            leds[(LED_LENGTH/2)-1] = CRGB::Black;
        }
        for(int i=0; i<(LED_LENGTH/2); i++){
            leds[(LED_LENGTH-i)-1] = leds[i];
        }
        framediv=0;
    }
}