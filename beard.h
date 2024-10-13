#ifndef BEARD_H
#define BEARD_H

#include "effect.h"
#include "particle.h"

class Beard : public Effect {
public:
    Particle particles[60];

    Beard(CRGB *leds);

    int nLEDS() override;
    void loadEffect(int variation) override;
    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    void handlePolyAT(int channel, int note, int value) override;
    void handleFrameUpdate();
};

#endif