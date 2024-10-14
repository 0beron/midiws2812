#include "bargraph.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"

Bargraph::Bargraph(CRGB *leds)
        : Effect(leds) {
}


void Bargraph::handleNoteOff(int channel, int note, int velocity) {
    FastLED.clear();    
}


void Bargraph::handleCC(int channel, int cc, int value) {
    CRGB col;
    if (cc == 2) {
        int v = (int)((value / 127.0f) * glb_maxleds);
        if (v > glb_maxleds) {
            v = glb_maxleds;
        }
        int s,e;
        if (channel==3) {
            s = 0;
            e = v;
        } else {
            e = glb_maxleds;
            s = glb_maxleds-v;
        }
        
        for (int i=s; i<e; i++) {
            if (channel==3){
                leds[i].green = 85;
            }
            else {
                leds[i].red = 85;
            }
        }

        if (channel==3) {
            s = v;
            e = glb_maxleds;
        } else {
            s = 0;
            e = glb_maxleds - v;
        }
        
        for (int i=s; i<e; i++) {
            if (channel==3){
                leds[i].green = 0;
            }
            else {
                leds[i].red = 0;
            }
        }

        if ((value%2 == 1) && v < (glb_maxleds)) {
            if (channel == 3) {
                leds[v].green = 30;
            } else {
                leds[glb_maxleds-(v+1)].red = 30;
            }
        }
    }

}