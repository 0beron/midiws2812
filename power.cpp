#include "power.h"
#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "WS2812.hpp"

static const uint8_t gRed_mW   = 16 * 5; ///< 16mA @ 5v = 80mW
static const uint8_t gGreen_mW = 11 * 5; ///< 11mA @ 5v = 55mW
static const uint8_t gBlue_mW  = 15 * 5; ///< 15mA @ 5v = 75mW
static const uint8_t gDark_mW  =  1 * 5; ///<  1mA @ 5v =  5mW
#define POWER_LED 25

/// Debug Option: Set to enable Serial debug statements for power limit functions
#define POWER_DEBUG_PRINT 0

// Power consumed by the MCU
static const uint8_t gMCU_mW  =  25 * 5; // 25mA @ 5v = 125 mW

static uint8_t  gMaxPowerIndicatorLEDPinNumber = 0; // default = Arduino onboard LED pin.  set to zero to skip this.


uint32_t calculate_unscaled_power_mW( const WS2812* ledbuffer, uint16_t numLeds ) //25354
{
    uint32_t red32 = 0, green32 = 0, blue32 = 0;
    const uint32_t* firstled = &(ledbuffer->data[0]);
    uint8_t* p = (uint8_t*)(firstled);
    uint8_t r,g,b;
    uint16_t count = numLeds;

    // This loop might benefit from an AVR assembly version -MEK
    while( count) {
        ledbuffer->getPixel(count,&r,&g,&b);
        red32   += r;
        green32 += g;
        blue32  += b;
        --count;
    }

    red32   *= gRed_mW;
    green32 *= gGreen_mW;
    blue32  *= gBlue_mW;

    red32   >>= 8;
    green32 >>= 8;
    blue32  >>= 8;

    uint32_t total = red32 + green32 + blue32 + (gDark_mW * numLeds);

    return total;
}


uint8_t calculate_max_brightness_for_power_vmA(const WS2812* ledbuffer, uint16_t numLeds, uint8_t target_brightness, uint32_t max_power_V, uint32_t max_power_mA) {
	return calculate_max_brightness_for_power_mW(ledbuffer, numLeds, target_brightness, max_power_V * max_power_mA);
}

uint8_t calculate_max_brightness_for_power_mW(const WS2812* ledbuffer, uint16_t numLeds, uint8_t target_brightness, uint32_t max_power_mW) {
 	uint32_t total_mW = calculate_unscaled_power_mW( ledbuffer, numLeds);

	uint32_t requested_power_mW = ((uint32_t)total_mW * target_brightness) / 256;

	uint8_t recommended_brightness = target_brightness;
	if(requested_power_mW > max_power_mW) { 
        recommended_brightness = (uint32_t)((uint8_t)(target_brightness) * (uint32_t)(max_power_mW)) / ((uint32_t)(requested_power_mW));
	}

	return recommended_brightness;
}


/*
void set_max_power_indicator_LED( uint8_t pinNumber)
{
    gMaxPowerIndicatorLEDPinNumber = pinNumber;
}

void set_max_power_in_volts_and_milliamps( uint8_t volts, uint32_t milliamps)
{
    FastLED.setMaxPowerInVoltsAndMilliamps(volts, milliamps);
}

void set_max_power_in_milliwatts( uint32_t powerInmW)
{
    FastLED.setMaxPowerInMilliWatts(powerInmW);
}

*/