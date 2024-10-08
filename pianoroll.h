#ifndef PIANOROLL_H
#define PIANOROLL_H
#include <FastLED.h>
#include "effect.h"
#include "particle.h"
#include "global.h"

class Pianoroll : public Effect {
public:
    
    uint8_t running = 0;
    uint8_t lastPrs = 0;
    int lastnote =0;
    uint32_t framediv = 0;
    uint8_t bl[LED_LENGTH];
    Pianoroll(CRGB *leds);

    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    void handleCC(int channel, int cc, int velocity) override;
    
    void handleFrameUpdate() override;
   
};

#endif