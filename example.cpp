#include <stdio.h>
#include <cmath>
#include "pico/stdlib.h"
//#include "WS2812.hpp"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/flash.h"
#include "particles.h"
#include "beatchaser.h"
#include "bargraph.h"
#include "pianoroll.h"
#include "streb.h"
#include "keys.h"
#include "FastLED.h"
#include "global.h"

#define LED_PIN 4
#define UART_ID uart1
#define BAUD_RATE 31250
#define UART_TX_PIN 8
#define UART_RX_PIN 9
#define BUTTON_PIN 17
#define MAXLEDS 60
//#define GPIO_FUNC_UART 2


#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

#define MIDI_NOTE_ON 0x90
#define NEFFECTS 7

typedef struct _message {
    unsigned char rstat;
    unsigned char status;
    unsigned char data1;
    unsigned char data2;
} message;

uint clock_counter = 0;
unsigned int count = 0;
unsigned int curr_effect = 0;

int brightness = 255;

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

int read_flash();
void write_flash(int value);

CRGB leds[MAXLEDS];

bool in_sysex = false;
bool flsh = false;
bool menu = false;
int bld=0;
CLEDController *control = nullptr;
//printf("%d", someint);


int main()
{
    unsigned char midichar = 0;
    unsigned char midilast = 0;
    int global_trigger;
    uint debounce = 100;
    unsigned int framecounter = 0;

    Effect *eff;
    message cmsg;

    config();

    control = &FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, MAXLEDS);
    gpio_put(BLED, 1);
        
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
    
    curr_effect = (unsigned int)read_flash();

    if (curr_effect >= NEFFECTS) {
        curr_effect = 0;
    }

    FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);

    eff = effects[curr_effect];
    loadEffect(*eff, vars[curr_effect]);
    //leds[0] = CRGB::Green;
    //ledStrip.fill(0);
    
    //FastLED.show();
    cmsg.rstat = 0;
    while (true) {

        midi_rx(&cmsg,eff);
          
        global_trigger = global_midi(&cmsg);
        if (debounce == 0 && (global_trigger == -2 ||
             !gpio_get(BUTTON_PIN))) {
            /* Any note on the control channel or a press
               of the button on GPIO will cycle effects, and
               write the settings to flash.
            */
            debounce = 30;
            curr_effect++;
            if (curr_effect >= NEFFECTS) {
                curr_effect = 0;
            }
            eff = effects[curr_effect];
            loadEffect(*eff, vars[curr_effect]);
            write_flash(curr_effect);
        }
        if (global_trigger >= 0) {
            /* A program change on the control channel
              picks an effect but does not persist the change.*/
            debounce = 30;
            curr_effect = global_trigger;
            eff = effects[curr_effect];
            loadEffect(*eff, vars[curr_effect]);
        }

        handle_midi(&cmsg, eff);

        framecounter++;
        if (framecounter == 25000) {
            
            eff->handleFrameUpdate();
            framecounter = 0;

            if (debounce > 0)  {
                debounce--;
                for (int ii=0; ii<LED_LENGTH; ii++) {
                    leds[ii] = 0;
                }
                for (int ii=0; ii<curr_effect+1; ii++) {
                    leds[ii] = CRGB(0,128,128);
                }
            }
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
    control->setLeds(leds, eff.nLEDS());
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
                
                if (clock_counter == 0) {
                    gpio_put(BLED, 1);
                }
                if (clock_counter == 11) {
                    gpio_put(BLED, 0);
                }

                
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

int global_midi(message* msg) {
    unsigned char channel;
    unsigned char stat;

    stat = msg->status & 0xF0;
    
    if (msg->rstat == 3) {
        channel = msg->status & 0xF;
        if (channel == GLOBAL_CHANNEL) {
            if (stat == 0x90 && msg->data2 > 0) {
                // Any note on
                // Cycle Effects
                return -2;
            }
        }
    } else if (msg->rstat == 2) {
        channel = msg->status & 0xF;
        if (channel == GLOBAL_CHANNEL) {
            if (stat == 0xC0) {
                // PC on global channel,
                // Pick effect by index if in range.
                if ((int)msg->data1 < NEFFECTS) {
                    return (int)msg->data1;
                }
            }
        }
    }
    return -1;
}


void handle_midi(message* msg, Effect *eff) {
    unsigned char channel;
    unsigned char stat;

    stat = msg->status & 0xF0;
    
    if (msg->rstat == 3) {
        
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
            // PC
            msg->rstat = 1;
        } else if (stat == 0xD0) {
            // Channel Pressure
            msg->rstat = 1;
        }
    }


}


int find_empty_page() {
    int *p, page, addr, lastval, newval;
    int first_empty_page = -1;
    for(page = 0; page < FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE; page++){
        addr = XIP_BASE + FLASH_TARGET_OFFSET + (page * FLASH_PAGE_SIZE);
        p = (int *)addr;
        /*Serial.print("First four bytes of page " + String(page, DEC) );
        Serial.print("( at 0x" + (String(int(p), HEX)) + ") = ");
        Serial.println(*p);*/
        if( *p == -1 && first_empty_page < 0){
            first_empty_page = page;
        //Serial.println("First empty page is #" + String(first_empty_page, DEC));
        }
    }
    return first_empty_page;
}

int read_flash() {
    int first_empty_page = -1;

    first_empty_page = find_empty_page();

    if (first_empty_page < 0) {
        first_empty_page = (FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE)-1;
    } else if (first_empty_page == 0) {
        return 0;
    }

    int *p, addr, value;

    // Compute the memory-mapped address, remembering to include the offset for RAM
    addr = XIP_BASE +  FLASH_TARGET_OFFSET + ((first_empty_page-1)*FLASH_PAGE_SIZE);
    p = (int *)addr; // Place an int pointer at our memory-mapped address
    value = *p; // Store the value at this address into a variable for later use
    return value;
}


void write_flash(int value) {
    int buf[FLASH_PAGE_SIZE/sizeof(int)];  // One page buffer of ints
    int *p, page, addr, lastval, newval;
    int first_empty_page = -1;

    first_empty_page = find_empty_page();
    *buf = value;

    if (first_empty_page < 0){
        // Serial.println("Full sector, erasing...");
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        first_empty_page = 0;
        restore_interrupts (ints);
    }
    //Serial.println("Writing to page #" + String(first_empty_page, DEC));
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(FLASH_TARGET_OFFSET + (first_empty_page*FLASH_PAGE_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts (ints);

}