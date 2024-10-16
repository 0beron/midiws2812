#ifndef PARTICLES_H
#define PARTICLES_H

#include "effect.h"
#include "particle.h"
#include "FastLED.h"

class Particles : public Effect {
public:

    int num = 10;
    int chance = 30;
    int fall = 4;
    int colour_scheme = 0;
    CRGB colour;

    Particle p[50];

    Particles(CRGB *leds)
        : Effect(leds) {

        }

    void loadEffect(int variation);
    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;

    void handleFrameUpdate() override;

};

#endif