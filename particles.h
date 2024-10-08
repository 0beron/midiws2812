#ifndef PARTICLES_H
#define PARTICLES_H

#include "effect.h"
#include "particle.h"
#include "FastLED.h"

class Particles : public Effect {
public:

    int num = 0;
    int chance = 10;

    Particle p[10];

    Particles(CRGB *leds)
        : Effect(leds) {

        }

    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;

    void handleFrameUpdate() override;

};

#endif