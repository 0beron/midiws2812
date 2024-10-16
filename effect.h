#ifndef EFFECT_H
#define EFFECT_H

#include <iostream>
#include <memory>
#include "FastLED.h"



class Effect {
public:

    Effect(CRGB *leds)
        : leds(leds) {}
    
    virtual ~Effect() = default;
    virtual void loadEffect(int variation);
    virtual int nLEDS();
    virtual void handleNoteOn(int channel, int note, int velocity);
    virtual void handleNoteOff(int channel, int note, int velocity);
    virtual void handlePolyAT(int channel, int note, int value);
    virtual void handleCC(int channel, int cc, int value);
    virtual void handleProgramChange(int channel, int value);
    virtual void handleClock(uint pulse_number);
    virtual void handleFrameUpdate();
    virtual void handleStop();
    virtual void handleStart();

protected:
    CRGB *leds;
    int variation;
};

#endif