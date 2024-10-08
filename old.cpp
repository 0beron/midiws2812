#include <MIDI.h>
#include <FastLED.h>
#include <colorutils.h>
#define LED_PIN     7
#define NUM_LEDS    60

#define numEffects 4
#define LED_WRITE_PIN 3
#define NOTE_ON_PIN 10
#define NOTE_OFF_PIN 12

CRGB leds[NUM_LEDS];
byte bl[NUM_LEDS];

/*CRGB ramp[255];*/

CRGBPalette16 palette;
volatile int rxBufferCount = 0;
volatile int maxRxBufferCount = 0;
MIDI_CREATE_DEFAULT_INSTANCE();
int i;
int j =0;
int iEffect;

int clockdivide;
int udiv;

bool running;

typedef void (*Handler) (void);
typedef void (*NoteHandler) (byte channel, byte pitch, byte velocity);


void checkBufferUsage() {
    int currentUsage = (SERIAL_RX_BUFFER_SIZE + rxBufferCount - Serial.available()) % SERIAL_RX_BUFFER_SIZE;
    if (currentUsage > maxRxBufferCount) {
        maxRxBufferCount = currentUsage;
    }
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
 
  if (channel == 2) {
    iEffect += 1;
    if (iEffect >= numEffects) {
      iEffect = 0;
    }
    setEffect(iEffect);
  }

}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  

}

void cc2led(byte channel, byte number, byte value) {
  CRGB col;
  if (number == 2) {
      digitalWrite(LED_BUILTIN, HIGH);
      int v = value / 2;
      if (v > NUM_LEDS) {
        v = NUM_LEDS;
      }
      int s,e;
      if (channel==3) {
        s =0;
        e = v-1;
      } else {
        e = NUM_LEDS-1;
        s = NUM_LEDS - v -1;
      }
      
      for (int i=s; i<e; i++) {
        if (channel==3){
          leds[i].green = 255;
        }
        else {
          leds[i].blue = 255;
        }
      }

      if (channel==3) {
        s = v;
        e = NUM_LEDS-1;
      } else {
        s = 0;
        e = NUM_LEDS - v;
      }
      
      for (int i=s; i<e; i++) {
         if (channel==3){
          leds[i].green = 0;
        }
        else {
          leds[i].blue = 0;
        }
      }
      FastLED.show();
      digitalWrite(LED_BUILTIN, LOW);
 }
  
}


void ignore_cc(byte channel, byte number, byte value) {

}

void handleStart() {
  clockdivide = 23;
  udiv=0;
  running=true;
}
void handleStop() {
  clockdivide = 23;
  udiv=0;
  running=false;
}

// Chaser 
void chase_update() {
   for (int ii=NUM_LEDS-1; ii>=0; ii--) {
      if (ii>0 && leds[ii].red < leds[ii-1].red) {
        leds[ii].red = leds[ii-1].red;
      } else if (leds[ii].red > 3) {
        leds[ii].red -= 3;
      } else {
        leds[ii].red = 0;
      }
   }
   /*leds[maxRxBufferCount / 5] = CRGB(0,255,0);*/
   FastLED.show();
}

void chase_handleClock() {
  if (!running) return;
  clockdivide++;
  if (clockdivide>23) {
    leds[0].red = 220;
    /*leds[maxRxBufferCount / 5] = CRGB(0,255,0);*/
    FastLED.show();
    clockdivide = 0;
    if (j>=NUM_LEDS) {
      j=0;
    }
  }
}


// Color Chaser
void c_chase_update() {
   udiv += 1;
   int h = (NUM_LEDS/2);
   if (udiv > 1000) {
     for (int ii=h-1; ii>=0; ii--) {
        if (ii>0 && bl[ii] < bl[ii-1]) {
          bl[ii] = bl[ii-1];
        } else if (bl[ii] > 3) {
          bl[ii] -= 3;
        } else {
          bl[ii] = 0;
        }
        leds[h-(ii+1)] = ColorFromPalette(palette, bl[ii]);
        leds[ii+h] = leds[h-(ii+1)];
     }
    /* leds[maxRxBufferCount / 5] = CRGB(0,255,0);*/
     FastLED.show();
     udiv = 0;
   }
}

void c_chase_handleClock() {
if (!running) return;
  clockdivide++;
  if (clockdivide>23) {
    bl[0] = 255;
    FastLED.show();
    clockdivide = 0;
    if (j>=NUM_LEDS) {
      j=0;
    }
  }
}


// Color Chaser
void cc2update() {
  
}

void ignore() {
  
}

// Mirror Notes
void mirrorNotes(byte channel, byte pitch, byte velocity)
{
if (channel == 2) {
handleNoteOn(channel, pitch, velocity);
return ;
}
   if (pitch > 32 && pitch < 92) {
     leds[pitch-32] = CRGB(0,255,255);
   }

   if (pitch == 48 || pitch == 52 || pitch == 55) {

       digitalWrite(NOTE_ON_PIN, HIGH);
       digitalWrite(NOTE_ON_PIN, LOW);
   } else {
       digitalWrite(NOTE_ON_PIN, HIGH);
       digitalWrite(NOTE_ON_PIN, LOW);
       digitalWrite(NOTE_ON_PIN, HIGH);
       digitalWrite(NOTE_ON_PIN, LOW);
    
   }
   
   digitalWrite(LED_WRITE_PIN, HIGH);
   FastLED.show();
   digitalWrite(LED_WRITE_PIN, LOW);
}

void mirrorNotesOff(byte channel, byte pitch, byte velocity)
{
   if (pitch > 32 && pitch < 92) {
     leds[pitch-32] = CRGB(0,0,0);
   }
   if (pitch == 48 || pitch == 52 || pitch == 55) {

       digitalWrite(NOTE_OFF_PIN, HIGH);
       digitalWrite(NOTE_OFF_PIN, LOW);
   } else {
       digitalWrite(NOTE_OFF_PIN, HIGH);
       digitalWrite(NOTE_OFF_PIN, LOW);
       digitalWrite(NOTE_OFF_PIN, HIGH);
       digitalWrite(NOTE_OFF_PIN, LOW);
    
   }

   digitalWrite(LED_WRITE_PIN, HIGH);
   FastLED.show();
   digitalWrite(LED_WRITE_PIN, LOW);
}

void setup_colour_table() {

  palette = CRGBPalette16(
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 0,0,0),
    CHSV( 134,255,0),
    CHSV( 134,255,85),
    CHSV( 134,255,171),
    CHSV( 134,255,255),
    CHSV( 143,255,255),
    CHSV( 152,255,255),
    CHSV( 160,255,255),
    CHSV( 181,255,255),
    CHSV( 202,255,255),
    CHSV( 224,255,255));
  
  /*fill_gradient(&(ramp[255-40]),40,CHSV(224, 255, 255), CHSV(224, 0, 255), SHORTEST_HUES);
  fill_gradient(&(ramp[255-80]),40,CHSV(160, 255, 255), CHSV(224, 255, 255), SHORTEST_HUES);
  fill_gradient(&(ramp[255-120]),40,CHSV(134,255,255), CHSV(160, 255, 255), SHORTEST_HUES);
  fill_gradient(&(ramp[255-160]),40,CHSV(134,255,0), CHSV(134, 255, 255), SHORTEST_HUES);
  fill_gradient(&ramp[30], (uint16_t)30, CHSV(64, 255, 255), CHSV(64, 0, 0), BACKWARD_HUES);*/
}

Handler update;

Handler updates[numEffects]      = {chase_update, c_chase_update, cc2update, ignore};
Handler clocks[numEffects]       = {chase_handleClock, c_chase_handleClock, ignore, ignore};
NoteHandler noteons[numEffects]  = {handleNoteOn, handleNoteOn, handleNoteOn, mirrorNotes};
NoteHandler noteoffs[numEffects] = {handleNoteOff, handleNoteOff, handleNoteOff, mirrorNotesOff};
NoteHandler ccs[numEffects]      = {ignore_cc, ignore_cc, cc2led, ignore_cc};

void setEffect(int i) {
  MIDI.setHandleNoteOn(noteons[i]);
  MIDI.setHandleNoteOff(noteoffs[i]);
  MIDI.setHandleClock(clocks[i]);
  MIDI.setHandleControlChange(ccs[i]);
  update = updates[i];
}

void setup() {
  iEffect = 0;
  delay(1000);
  Serial.begin(31250);
  // put your setup code here, to run once:
  pinMode(NOTE_ON_PIN, OUTPUT); 
  pinMode(LED_WRITE_PIN, OUTPUT);
  pinMode(NOTE_OFF_PIN, OUTPUT); 
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(LED_BUILTIN, OUTPUT);

  MIDI.setHandleStart(handleStart);
  MIDI.setHandleStop(handleStop);
  
  i = 0;
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.turnThruOff();
  setup_colour_table();
  clockdivide = 23;
  leds[0] = CRGB(0,0,0);
  bl[0] = 0;
  maxRxBufferCount = 0;
  setEffect(0);
}

void loop() {
  // put your main code here, to run repeatedly:
    // Call MIDI.read the fastest you can for real-time performance.
    MIDI.read();
    /*checkBufferUsage();*/
    if (running) {
      (*update)();
    }
}
