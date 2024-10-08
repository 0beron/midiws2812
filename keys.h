#ifndef KEYS_H
#define KEYS_H

#include "effect.h"
#include "particle.h"

class Keys : public Effect {
public:
    Particle particles[60];

    Keys(CRGB *leds);

    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    void handlePolyAT(int channel, int note, int value) override;
    void handleFrameUpdate();
};

#endif