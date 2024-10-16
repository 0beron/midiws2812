#define BLED 25
#define NEFFECTS 9
#define NSETTINGS 5

#define MAXLEDS 180

#define EFFECT 0
#define BRIGHTNESS 1
#define CHORD_OFFSET 2
#define BASS_OFFSET 3
#define INTERPOLATOR 4

#ifndef GLOBAL_H
#define GLOBAL_H

extern int settings[NSETTINGS];

extern int glb_outleds;
extern int glb_maxleds;
extern int glb_jumper_val;
extern int glb_control_channel;
extern int glb_note_channel;

void debug(int val, CRGB col);


#endif                