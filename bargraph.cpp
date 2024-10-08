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
        int v = value / 2;
        if (v > LED_LENGTH) {
            v = LED_LENGTH;
        }
        int s,e;
        if (channel==3) {
            s = 0;
            e = v;
        } else {
            e = LED_LENGTH;
            s = LED_LENGTH-v;
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
            e = LED_LENGTH;
        } else {
            s = 0;
            e = LED_LENGTH - v;
        }
        
        for (int i=s; i<e; i++) {
            if (channel==3){
                leds[i].green = 0;
            }
            else {
                leds[i].red = 0;
            }
        }

        if ((value%2 == 1) && v < (LED_LENGTH)) {
            if (channel == 3) {
                leds[v].green = 30;
            } else {
                leds[LED_LENGTH-(v+1)].red = 30;
            }
        }


    }

}