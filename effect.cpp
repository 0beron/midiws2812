
#include "effect.h"

void Effect::loadEffect(int variation) {
    this->variation = variation;
}
int  Effect::nLEDS() {
    return 60;
}
void Effect::handleNoteOn(int channel, int note, int velocity) {}
void Effect::handleNoteOff(int channel, int note, int velocity) {}
void Effect::handleCC(int channel, int cc, int value) {}
void Effect::handleProgramChange(int channel, int value) {}
void Effect::handlePolyAT(int channel, int note, int value) {}
void Effect::handleClock(uint pulse_number) {}
void Effect::handleFrameUpdate() {}
void Effect::handleStart() {}
void Effect::handleStop() {}
