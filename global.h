#define LED_LENGTH 60
#define BLED 25
#define GLOBAL_CHANNEL 0xE
#define LOCAL_CHANNEL 0xB

#define NEFFECTS 7

#define NSETTINGS 4

#define EFFECT 0
#define BRIGHTNESS 1
#define CHORD_OFFSET 2
#define BASS_OFFSET 3

#ifndef GLOBAL_H
#define GLOBAL_H
extern int brightness;


void debug(int val, CRGB col);





#endif                