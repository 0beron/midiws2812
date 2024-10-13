#include "beatchaser.h"
#include <algorithm>
#include "FastLED.h"
#include "particle.h"
#include "render.h"
#include "global.h"

Beatchaser::Beatchaser(CRGB *leds)
        : Effect(leds) {
    for (int i=0; i<LED_LENGTH;i++) {
        bl[i]=0;
    }
   /* palette = CRGBPalette16(
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 134,255,0),
    CHSV( 134,255,85),
    CHSV( 134,255,171),
    CHSV( 134,255,255),
    CHSV( 143,255,255),
    CHSV( 152,255,255),
    CHSV( 160,255,255),
    CHSV( 181,255,255),
    CHSV( 202,255,255),
    CHSV( 224,255,255));*/


 palette = CRGBPalette16(
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 134,255,0),
    CHSV( 134,255,85),
    CHSV( 134,255,171),
    CHSV( 134,255,255),
    CHSV( 143,255,255),
    CHSV( 152,255,255),
    CHSV( 160,255,255),
    CHSV( 181,255,255),
    CHSV( 202,255,255),
    CHSV( 224,255,255),
    CHSV( 224,128,255),
    CHSV( 224,0,255)
    );

palette2 = CRGBPalette16(
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 60,255,0),
    CHSV( 60,255,64),
    CHSV( 60,255,128),
    CHSV( 60,255,192),
    CHSV( 40,255,255),
    CHSV( 20,255,255),
    CHSV( 0,255,255),
    CHSV( 224,128,255),
    CHSV( 224,0,255)
    );
}


int Beatchaser::nLEDS() {
    return 60;
}

/*void Beatchaser::loadEffect(int variation) {
    this->variation = variation;
}*/

void Beatchaser::handleNoteOn(int channel, int note, int velocity) {
    if (variation == 1 && channel == LOCAL_CHANNEL){
        bl[0] = 255;
    }
}

void Beatchaser::handleNoteOff(int channel, int note, int velocity) {

}

void Beatchaser::handleStart() {
    running=true;
    
}
void Beatchaser::handleStop() {
    running=false;
   
}

void Beatchaser::handleFrameUpdate() {
    //FastLED.clear();
   int h = (LED_LENGTH/2);

    for (int ii=h-1; ii>=0; ii--) {
        if (ii>0 && bl[ii] < bl[ii-1]) {
            bl[ii] = bl[ii-1];
        } else if (bl[ii] > 3) {
            bl[ii] -= 3;
        } else {
            bl[ii] = 0;
        }
        if (variation == 0) {
            leds[h-(ii+1)] = ColorFromPalette(palette, bl[ii]);
        } else {
            leds[h-(ii+1)] = ColorFromPalette(palette2, bl[ii]);
        }
        leds[ii+h] = leds[h-(ii+1)];
    }
    /* leds[maxRxBufferCount / 5] = CRGB(0,255,0);*/
    //FastLED.show();
}

void Beatchaser::handleClock(uint pulse_number) {
    if (variation==0){
        if (!running) return;
        if (pulse_number == 0) {
        
            bl[0] = 255;
            //FastLED.show();
        }
        if (pulse_number == 11) {
            
        }
    }
}