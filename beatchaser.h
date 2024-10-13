#ifndef BEATCHASER_H
#define BEATCHASER_H
#include <FastLED.h>
#include "effect.h"
#include "particle.h"
#include "global.h"

class Beatchaser : public Effect {
public:
    CRGBPalette16 palette;
    CRGBPalette16 palette2;
    bool running = false;
    uint8_t bl[LED_LENGTH];
    int trigger = 0;
    Beatchaser(CRGB *leds);

    int nLEDS() override;
    //void loadEffect(int variation) override;
    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    
    void handleFrameUpdate() override;
    void handleClock(uint pulse_number) override;
    void handleStart() override;
    void handleStop() override;
};

#endif