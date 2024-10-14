#include <stdio.h>
#include <cmath>

#include "pico/stdlib.h"
//#include "WS2812.hpp"
#include "hardware/uart.h"
#include "hardware/gpio.h"

#include "particles.h"
#include "beatchaser.h"
#include "bargraph.h"
#include "pianoroll.h"
#include "streb.h"
#include "keys.h"
#include "FastLED.h"
#include "global.h"
#include "globaleffect.h"

#define LED_PIN 4
#define UART_ID uart1
#define BAUD_RATE 31250
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define BUTTON_PIN 17
#define MAXLEDS 180
//#define GPIO_FUNC_UART 2

#define MIDI_NOTE_ON 0x90

typedef struct _message {
    unsigned char rstat;
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
} message;

uint clock_counter = 0;
unsigned int count = 0;

Effect *effects[NEFFECTS];
int vars[NEFFECTS];

// function declarations
void config();

void midi_rx(message* msg, Effect* eff);
void midi_tx(unsigned char);
void handle_midi(message* msg, Effect* eff);
int global_midi(message* msg);

void note_on(unsigned char channel, unsigned char note, unsigned char vel);
void note_off(unsigned char channel, unsigned char note, unsigned char vel);

void loadEffect(Effect &eff, int variation);

CRGB leds[MAXLEDS];

bool in_sysex = false;
bool flsh = false;
bool menu = false;
int bld=0;
CLEDController *control = nullptr;

void debug(int val, CRGB col) {
    FastLED.clear();
    int i = 0;
    while (val > 0) {
        if (val & 0x1) {
            leds[i] = col;
        }
        val >>= 1;
        i++;
    }
    FastLED.show();
    sleep_ms(1500);
}


int main()
{
    unsigned char midichar = 0;
    unsigned char midilast = 0;
    int global_trigger;
    uint debounce = 100;
    unsigned int framecounter = 0;

    Effect *eff;
    
    message cmsg, gmsg;

    config();

    control = &FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, MAXLEDS);

    GlobalEffect *global_effect = new GlobalEffect(leds);

    // Effects:
    effects[0] = new Pianoroll(leds);
    effects[1] = new Keys(leds);
    effects[2] = new Particles(leds);
    effects[3] = new Beatchaser(leds);
    effects[4] = new Bargraph(leds);
    effects[5] = new Streb(leds);
    effects[6] = new Beatchaser(leds);

    vars[0] = 0;
    vars[1] = 0;
    vars[2] = 0;
    vars[3] = 0;
    vars[4] = 0;
    vars[5] = 0;
    vars[6] = 1;
 
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);

    eff = effects[settings[EFFECT]];
    loadEffect(*eff, vars[settings[EFFECT]]);
    
    cmsg.rstat = 0;
    while (true) {

        midi_rx(&cmsg,eff);
          
        if (global_effect->debounce == 0 && !gpio_get(BUTTON_PIN)) {
            global_effect->handleNoteOn(GLOBAL_CHANNEL, 64, 100);
            global_effect->change_timer = 200;
        }

        gmsg.rstat = cmsg.rstat;
        gmsg.status = cmsg.status;
        gmsg.data1 = cmsg.data1;
        gmsg.data2 = cmsg.data2;
        handle_midi(&gmsg, global_effect);

        if (settings[EFFECT] != global_effect->prev_settings[EFFECT]) {
            eff = effects[settings[EFFECT]];
            loadEffect(*eff, vars[settings[EFFECT]]);
            global_effect->copySettings();
        }

        handle_midi(&cmsg, eff);

        framecounter++;
        if (framecounter == 15000) {
            
            eff->handleFrameUpdate();
            global_effect->handleFrameUpdate();
            framecounter = 0;

            /* for(int i = 59; i>0; i--) {
                leds[i*3] = leds[i];
                leds[i] = CRGB::Black;
            } */

            FastLED.show();
        }
    }

    return 0;
}

void config () {
    stdio_init_all();
    setup_default_uart();
    uart_init(UART_ID, BAUD_RATE);

	// set UART Tx and Rx pins
	gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
	gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    gpio_init(BLED);
    gpio_set_dir(BLED, GPIO_OUT);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    uart_set_fifo_enabled(UART_ID, true);

	// disable cr/lf conversion on Tx
	uart_set_translate_crlf(UART_ID, false);
}

void loadEffect(Effect &eff, int variation) {
    eff.loadEffect(variation);
    //control->setLeds(leds, eff.nLEDS());
}

void midi_rx(message* msg, Effect* eff) {
	if(uart_is_readable(UART_ID)) {
		unsigned char mc = uart_getc (UART_ID);

        if (mc >= 0xF0) {
            if (mc == 0xFF) {
                // Reset
            } else if (mc == 0xFE) {
                // Active sense
            } else if (mc == 0xFC) {
                // Stop
                eff->handleStop();
                
            } else if (mc == 0xFB) {
                // Continue
            } else if (mc == 0xFA) {
                //Start
                eff->handleStart();
                eff->handleClock(0);
                clock_counter = 0;
            } else if (mc == 0xF8) {
                // 24 PPQN clock
                clock_counter++;
                if (clock_counter > 23) {
                    clock_counter = 0;
                }
                eff->handleClock(clock_counter);
                
                /*if (clock_counter == 0) {
                    gpio_put(BLED, 1);
                }
                if (clock_counter == 11) {
                    gpio_put(BLED, 0);
                }*/

                
            } else if (mc == 0xF0) {
                in_sysex = true;
            } else if (mc == 0xF7) {
                in_sysex = false;
            }
            // Sysex also falls in here, ignored.
        }
        
        if (!in_sysex) {
            if (mc >= 0x80) {
                // Status byte
                msg->status = mc;
                msg->rstat = 1;
            } else {
                // Data byte
                if (msg->rstat == 1) {
                    msg->data1 = mc;
                    msg->rstat = 2;
                } else {
                    msg->data2 = mc;
                    msg->rstat = 3;
                }
            }
        }
   
    }
}

void handle_midi(message* msg, Effect *eff) {
    unsigned char channel;
    unsigned char stat;

    stat = msg->status & 0xF0;
    
    if (msg->rstat == 3) {
        printf("%02X %02X %02X\n", msg->status, msg->data1, msg->data2);
        
        printf("EFF ADDR %x\n", eff);

        channel = msg->status & 0xF;
        if (stat == 0x90) {
            if (msg->data2 == 0) {
                eff->handleNoteOff(channel, msg->data1, msg->data2);
            } else {
                eff->handleNoteOn(channel, msg->data1, msg->data2);
            }
        } else if (stat == 0x80) {
            eff->handleNoteOff(channel, msg->data1, msg->data2);
        } else if (stat == 0xA0) {
            eff->handlePolyAT(channel, msg->data1, msg->data2);
        } else if (stat == 0xB0) {
             eff->handleCC(channel, msg->data1, msg->data2);
        } else if (stat == 0xE0) {
            // Pitch Bend
        }
        msg->rstat = 1;
    } else if (msg->rstat == 2) {
        channel = msg->status & 0xF;
        if (stat == 0xC0) {
            printf("%02X %02X\n", msg->status, msg->data1);
            eff->handleProgramChange(channel, msg->data1);
            msg->rstat = 1;
        } else if (stat == 0xD0) {
            // Channel Pressure
            msg->rstat = 1;
        }
    }


}


