#ifndef GLOBALEFFECT_H
#define GLOBALEFFECT_H

#include "effect.h"
#include "global.h"

class GlobalEffect : public Effect {
public:
    GlobalEffect(CRGB *leds);

    //int settings[NSETTINGS];
    int prev_settings[NSETTINGS];
    
    int debounce = 0;
    int change_timer = 0;

    void copySettings();

    void handleNoteOn(int channel, int note, int velocity) override;
    void handleCC(int channel, int cc, int value) override;
    void handleProgramChange(int channel, int value) override;
    void handleFrameUpdate() override;
};

#endif 