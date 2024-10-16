#include "WS2812.hpp"
#include "WS2812.pio.h"
#include <algorithm>
#include <cmath>
#include "power.h"
//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

uint8_t gamma_table[255];

WS2812::WS2812(uint pin, uint length, PIO pio, uint sm)  {
    initialize(pin, length, pio, sm, NONE, GREEN, RED, BLUE);
}

WS2812::WS2812(uint pin, uint length, PIO pio, uint sm, DataFormat format) {
    switch (format) {
        case FORMAT_RGB:
            initialize(pin, length, pio, sm, NONE, RED, GREEN, BLUE);
            break;
        case FORMAT_GRB:
            initialize(pin, length, pio, sm, NONE, GREEN, RED, BLUE);
            break;
        case FORMAT_WRGB:
            initialize(pin, length, pio, sm, WHITE, RED, GREEN, BLUE);
            break;
    }
}

WS2812::WS2812(uint pin, uint length, PIO pio, uint sm, DataByte b1, DataByte b2, DataByte b3) {
    initialize(pin, length, pio, sm, b1, b1, b2, b3);
}

WS2812::WS2812(uint pin, uint length, PIO pio, uint sm, DataByte b1, DataByte b2, DataByte b3, DataByte b4) {
    initialize(pin, length, pio, sm, b1, b2, b3, b4);
}

WS2812::~WS2812() {
    
}

void compute_gamma_correction_table(double gamma) {
    for (int i = 0; i < 255; ++i) {
        gamma_table[i] = static_cast<uint8_t>(std::round(255 * std::pow(i / 255.0, gamma)));
    }
}


void WS2812::initialize(uint pin, uint length, PIO pio, uint sm, DataByte b1, DataByte b2, DataByte b3, DataByte b4) {
    this->pin = pin;
    this->length = length;
    this->pio = pio;
    this->sm = sm;
    this->data = new uint32_t[length];
    this->bytes[0] = b1;
    this->bytes[1] = b2;
    this->bytes[2] = b3;
    this->bytes[3] = b4;
    uint offset = pio_add_program(pio, &ws2812_program);
    uint bits = (b1 == NONE ? 24 : 32);
    compute_gamma_correction_table(1.7);
    #ifdef DEBUG
    printf("WS2812 / Initializing SM %u with offset %X at pin %u and %u data bits...\n", sm, offset, pin, bits);
    #endif
    ws2812_program_init(pio, sm, offset, pin, 800000, bits);
}

uint32_t WS2812::convertData(uint32_t rgbw) {
    uint32_t result = 0;
    for (uint b = 0; b < 4; b++) {
        switch (bytes[b]) {
            case RED:
                result |= (rgbw & 0xFF);
                break;
            case GREEN:
                result |= (rgbw & 0xFF00) >> 8;
                break;
            case BLUE:
                result |= (rgbw & 0xFF0000) >> 16;
                break;
            case WHITE:
                result |= (rgbw & 0xFF000000) >> 24;
                break;
        }
        result <<= 8;
    }
    return result;
}

void WS2812::setPixelColor(uint index, uint32_t color) {
    if (index < length) {
        data[index] = convertData(color);
    }
}

void WS2812::setPixelColor(uint index, uint8_t red, uint8_t green, uint8_t blue) {
    setPixelColor(index, RGB(red, green, blue));
}

void WS2812::maxPixelColor(uint index, uint8_t red, uint8_t green, uint8_t blue) {
    if (index < length) {
        maxPixelColor(index, RGB(red, green, blue));
    }
}

void WS2812::maxPixelColor(uint index, uint32_t raw) {
    if (index < length) {
        uint32_t conv = convertData(raw);
        uint32_t mx = 0;
        uint32_t mask = 0xFF;
        for (int i=0; i<4; i++) {
            mx |= std::max(conv&mask,data[index]&mask);
            mask <<= 8;
        }
        data[index] = mx;
    }
}

uint32_t applyGamma(uint32_t in) {
    return gamma_table[(in&0xFF)] | (gamma_table[(in&0xFF00)>>8]<<8)| (gamma_table[(in&0xFF0000)>>16]<<16)| (gamma_table[(in&0xFF000000)>>24]<<24);
}

uint32_t scale(uint32_t in, uint8_t maxbri) {
    uint32_t mb = (uint32_t)maxbri;
    uint32_t out = 0;
    uint8_t* bytes = reinterpret_cast<uint8_t*>(&in);
    uint8_t* b = reinterpret_cast<uint8_t*>(&out);
    for (int i=0;i<4;i++) {
        b[i] = (bytes[i]*mb)>>8;
    }
    return out;
}



void WS2812::getPixel(uint index, uint8_t *red, uint8_t *green, uint8_t *blue) const {
    if (index < length) {
        uint8_t shift = 24;
        for (uint b = 0; b < 4; b++) {
            switch (bytes[b]) {
            case RED:
                *red = (data[index]&(0xFF<<shift)) >> shift;
                break;
            case GREEN:
                *green = (data[index]&(0xFF<<shift)) >> shift;
                break;
            case BLUE:
                *blue = (data[index]&(0xFF<<shift)) >> shift;
                break;
            }
            shift -= 8;
        }
    }
}


void WS2812::setPixelColor(uint index, uint8_t red, uint8_t green, uint8_t blue, uint8_t white) {
    setPixelColor(index, RGBW(red, green, blue, white));
}

void WS2812::fill(uint32_t color) {
    fill(color, 0, length);
}

void WS2812::fill(uint32_t color, uint first) {
    fill(color, first, length-first);
}

void WS2812::fill(uint32_t color, uint first, uint count) {
    uint last = (first + count);
    if (last > length) {
        last = length;
    }
    color = convertData(color);
    for (uint i = first; i < last; i++) {
        data[i] = color;
    }
}

void WS2812::show() {
    #ifdef DEBUG
    for (uint i = 0; i < length; i++) {
        printf("WS2812 / Put data: %08X\n", data[i]);
    }
    #endif

    /*uint32_t power = calculate_unscaled_power_mW(this, length);*/
    uint8_t maxbri = calculate_max_brightness_for_power_vmA(this, length, 255, 5, 300);

    for (uint i = 0; i < length; i++) {
        pio_sm_put_blocking(pio, sm, applyGamma(scale(data[i],maxbri)));
    }   
}