#ifndef POWER_H
#define POWER_H

#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
#include "WS2812.hpp"

void set_max_power_in_volts_and_milliamps( uint8_t volts, uint32_t milliamps);
void set_max_power_in_milliwatts( uint32_t powerInmW);

void set_max_power_indicator_LED( uint8_t pinNumber); // zero = no indicator LED

void show_at_max_brightness_for_power();
void delay_at_max_brightness_for_power( uint16_t ms);

/// Determines how many milliwatts the current LED data would draw
/// at max brightness (255)
/// @param ledbuffer the LED data to check
/// @param numLeds the number of LEDs in the data array
/// @returns the number of milliwatts the LED data would consume at max brightness
uint32_t calculate_unscaled_power_mW(const WS2812 *ledbuffer, uint16_t numLeds);

/// Determines the highest brightness level you can use and still stay under
/// the specified power budget for a given set of LEDs.
/// @param ledbuffer the LED data to check
/// @param numLeds the number of LEDs in the data array
/// @param target_brightness the brightness you'd ideally like to use
/// @param max_power_mW the max power draw desired, in milliwatts
/// @returns a limited brightness value. No higher than the target brightness,
/// but may be lower depending on the power limit.
uint8_t calculate_max_brightness_for_power_mW(const WS2812* ledbuffer, uint16_t numLeds, uint8_t target_brightness, uint32_t max_power_mW);

/// @copybrief calculate_max_brightness_for_power_mW()
/// @param ledbuffer the LED data to check
/// @param numLeds the number of LEDs in the data array
/// @param target_brightness the brightness you'd ideally like to use
/// @param max_power_V the max power in volts
/// @param max_power_mA the max power in milliamps
/// @returns a limited brightness value. No higher than the target brightness,
/// but may be lower depending on the power limit.
uint8_t calculate_max_brightness_for_power_vmA(const WS2812* ledbuffer, uint16_t numLeds, uint8_t target_brightness, uint32_t max_power_V, uint32_t max_power_mA);

/// Determines the highest brightness level you can use and still stay under
/// the specified power budget for all sets of LEDs. 
/// Unlike the other internal power functions which use a pointer to a
/// specific set of LED data, this function uses the ::CFastLED linked list
/// of LED controllers and their attached data.
/// @param target_brightness the brightness you'd ideally like to use
/// @param max_power_mW the max power draw desired, in milliwatts
/// @returns a limited brightness value. No higher than the target brightness,
/// but may be lower depending on the power limit.
uint8_t  calculate_max_brightness_for_power_mW( uint8_t target_brightness, uint32_t max_power_mW);

/// @} PowerInternal


/// @} Power

#endif