#ifndef BARGRAPH_H
#define BARGRAPH_H

#include "effect.h"
#include "particle.h"

class Bargraph : public Effect {
public:
    Bargraph(CRGB *leds);

    void handleNoteOff(int channel, int note, int velocity) override;
    void handleCC(int channel, int cc, int velocity) override;
    
};

#endif 