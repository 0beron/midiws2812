#ifndef RENDER_H
#define RENDER_H
//#include "WS2812.hpp"
#include "FastLED.h"
#include "particle.h"
#include <memory>

void drawParticle(CRGB *leds, Particle p);

#endif