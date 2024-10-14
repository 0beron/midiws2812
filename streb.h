#ifndef STREB_H
#define STREB_H

#include "effect.h"
#include "particle.h"

class Streb : public Effect {
public:
    Streb(CRGB *leds);
    Particle particles[60];
    CRGB ledbuff[60];
    
    uint8_t running = 0;
    uint8_t lastPrs = 0;
    int lastnote = 0;
    uint32_t framediv = 0;
    

    int nLEDS() override;
    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    void handleCC(int channel, int cc, int velocity) override;
    void handleFrameUpdate();
};

#endif 