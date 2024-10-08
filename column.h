#ifndef Column_H
#define Column_H

#include "effect.h"
#include "particle.h"

class Column : public Effect {
public:

    Column(std::shared_ptr<WS2812> leds);

    void handleNoteOn(int channel, int note, int velocity) override;
    void handleNoteOff(int channel, int note, int velocity) override;
    void handlePolyAT(int channel, int note, int value) override;
    void handleFrameUpdate();
};

#endif