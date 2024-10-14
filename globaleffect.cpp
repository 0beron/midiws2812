#include "globaleffect.h"
#include "global.h"
#include "pico/stdlib.h"
#include "hardware/flash.h"
#define CHANGE_FRAMES 200

#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)

int settings[NSETTINGS];

static void vclamp(int *val, int min, int max);
static void read_flash2(int *buffer, int count);
static void write_flash(int *buffer, int count);
static int find_empty_page();

GlobalEffect::GlobalEffect(CRGB *leds)
        : Effect(leds) {

    read_flash2(settings,4);    
    vclamp(&(settings[EFFECT]), 0, NEFFECTS-1);
    vclamp(&(settings[BRIGHTNESS]), 12, 255);
    vclamp(&(settings[CHORD_OFFSET]), -63, 63);
    vclamp(&(settings[BASS_OFFSET]), -63, 63);

    printf("Brightness at start %d\n", settings[BRIGHTNESS]);

    FastLED.setBrightness((uint8_t)settings[BRIGHTNESS]);
}

void GlobalEffect::copySettings() {
    for (int i=0;i<NSETTINGS;i++) {
        prev_settings[i] = settings[i];
    }
}


void GlobalEffect::handleNoteOn(int channel, int note, int velocity) {
    
    if (channel == glb_control_channel) {
        printf("Manual Cycle\n");
        // Manual effect cycle. Show index number for 30 frames
        debounce = 30;
        // Start the timer to write flash.
        change_timer = CHANGE_FRAMES;

        // Stash the old settings so main loop can tell something
        // changed and load the next effect.
        copySettings();
        
        // Increment the effect index and wrap
        settings[EFFECT]++;
        if (settings[EFFECT] >= NEFFECTS) {
            settings[EFFECT] = 0;
        }
    }

    /* Streb offests */
    if (channel == 4) {
        copySettings();
        settings[CHORD_OFFSET] = (64-note);
    }
    if (channel == 5) {
        copySettings();
        settings[BASS_OFFSET] = (64-note);
    }

}

void GlobalEffect::handleCC(int channel, int cc, int value) {
    if (cc == 91) {
        // CC 91 sets Brightness
        change_timer = CHANGE_FRAMES;
        settings[BRIGHTNESS] = 2*value;
        if (settings[BRIGHTNESS] > 255) { settings[BRIGHTNESS] = 255;}
        FastLED.setBrightness((uint8_t)value);
    }
}

void GlobalEffect::handleProgramChange(int channel, int value) {
    if (channel == glb_control_channel && value < NEFFECTS) {
        // Program change sets an effect by index, and does not
        // display anything or trigger a flash write.
        copySettings();
        settings[EFFECT] = value;
        printf("Effect set by PC to %d\n", value);
        printf("Settings[EFFECT] now %d\n", settings[EFFECT]);
    }
}

void GlobalEffect::handleFrameUpdate() {

    if (change_timer > 0) {
        // Debug indication that flash write is ocurring.
        change_timer--;
        if (change_timer == 1) {
            printf("FLASH WRITE....\n");
            write_flash(settings, 4);
            char buffer[128];  // Buffer to hold the formatted string
            sprintf(buffer, "Value 1: %d\nValue 2: %d\nValue 3: %d\nValue 4: %d\n",
                settings[0], settings[1], settings[2], settings[3]);

            printf("%s", buffer);
        }
    }

    if (debounce > 0)  {
        // Show the index number of the effect
        FastLED.clear();
        debounce--;
        for (int ii=0; ii<glb_maxleds; ii++) {
            leds[ii] = 0;
        }
        for (int ii=0; ii<settings[EFFECT]+1; ii++) {
            leds[ii] = CRGB(0,128,128);
        }
    }
}

void vclamp(int *val, int min, int max) {
    if (*val < min ) {*val = min;}
    if (*val > max ) {*val = max;}
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

void read_flash2(int *buffer, int count) {
    if (count <= 0 || buffer == nullptr) {
        return;  // Return if count is non-positive or pointer is null
    }

    int first_empty_page = -1;

    first_empty_page = find_empty_page();

    if (first_empty_page < 0) {
        first_empty_page = (FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE)-1;
    } else if (first_empty_page == 0) {
        return;
    }

    // Compute the memory-mapped address, remembering to include the offset for RAM
    int addr = XIP_BASE +  FLASH_TARGET_OFFSET + ((first_empty_page-1)*FLASH_PAGE_SIZE);
    
    // Calculate how many ints can be read
    int read_count = (count > (FLASH_PAGE_SIZE / sizeof(int))) ? (FLASH_PAGE_SIZE / sizeof(int)) : count;
    printf("Reading %d from %x\n", read_count, addr);
    // Read data from flash into the buffer
    for (int i = 0; i < read_count; i++) {
        buffer[i] = *((int *)(addr + (i * sizeof(int))));
    }
    

    char msg[128];  // Buffer to hold the formatted string
    sprintf(msg, "Value 1: %d\nValue 2: %d\nValue 3: %d\nValue 4: %d\n",
        buffer[0], buffer[1], buffer[2], buffer[3]);

    printf("%s", msg);

    //p = (int *)addr; // Place an int pointer at our memory-mapped address

}

void write_flash(int *values, int count) {
    if (count <= 0 || values == nullptr) {
        return;  // Return if count is non-positive or pointer is null
    }
    int buf[FLASH_PAGE_SIZE/sizeof(int)];  // One page buffer of ints
    int *p, page, addr, lastval, newval;
    int first_empty_page = -1;

    first_empty_page = find_empty_page();
    //*buf = value;

    if (first_empty_page < 0){
        // Serial.println("Full sector, erasing...");
        uint32_t ints = save_and_disable_interrupts();
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
        first_empty_page = 0;
        restore_interrupts (ints);
    }

    // Ensure we do not exceed the page capacity
    int space_left = FLASH_PAGE_SIZE / sizeof(int);
    int to_write = (count > space_left) ? space_left : count;

    // Fill buffer with the integers to be written
    for (int i = 0; i < to_write; i++) {
        buf[i] = values[i];
    }

    // Zero out the remaining space in the buffer if less than page size
    for (int i = to_write; i < space_left; i++) {
        buf[i] = 0; // Optional: Clear remaining space to avoid garbage data
    }

    //Serial.println("Writing to page #" + String(first_empty_page, DEC));
    uint32_t ints = save_and_disable_interrupts();
    printf("Writing %d from %x\n", to_write, FLASH_TARGET_OFFSET + (first_empty_page*FLASH_PAGE_SIZE));
    flash_range_program(FLASH_TARGET_OFFSET + (first_empty_page*FLASH_PAGE_SIZE), (uint8_t *)buf, FLASH_PAGE_SIZE);
    restore_interrupts (ints);

}