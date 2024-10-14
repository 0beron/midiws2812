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
    printf("piano roll note on\n");
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
        for(int i=0; i<(glb_maxleds/2); i++){
            leds[i] = leds[i+1];
        }
        if (running > 0) {
            leds[(glb_maxleds/2)-1] = CHSV((lastnote%12)*21,255, lastPrs*2);
        } else {
            leds[(glb_maxleds/2)-1] = CRGB::Black;
        }
        for(int i=0; i<(glb_maxleds/2); i++){
            leds[(glb_maxleds-i)-1] = leds[i];
        }
        framediv=0;
    }
}