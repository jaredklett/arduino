#include "LowPower.h"

#include <Adafruit_NeoPixel.h>

const uint8_t mycolors[][3] PROGMEM = {
  {252,23,218}, // pink
  {255,153,0},  // orange
  {255,0,255},  // magenta
  {0,0,255}, // blue
  {0,255,0}, // green
  {0,255,255}, // cyan
  {255,255,0}, // yellow
  {72,209,204} // turqoise
};

#define LED_PIN 1
#define PIR_PIN 3
#define PCELL_PIN A1
#define RAND_PIN A10

#define NUM_PIXELS 12
#define BG 1
#define NUM_COLORS 8

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

int color;
int center = 0;
int step = -1;
int maxSteps = 16;
float fadeRate = 0.6;
int diff;

int photocellPin = A0;
boolean nighttime = false;

//background color
uint32_t currentBg = random(256);
uint32_t nextBg = currentBg;

void setup() {
  randomSeed(analogRead(RAND_PIN));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // check light sensor
  // if it's nighttime and getting light,turn off leds
  if (nighttime && !isDark()) {
    nighttime = false;
    clearStrip();
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    return;
  }
  // sleep during the day
  if (!nighttime && !isDark()) {
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
    return;
  }
  // if it's daytime and getting dark, turn on leds
  if (!nighttime && isDark()) {
    nighttime = true;
  }

  // check pir sensor
  // if no motion detected, breath
  // if motion detected and within timeout, ripple
  if (motionDetected()) {
    long randomNumber = random(3);
    switch (randomNumber) {
      case 0:
        ripple();
        break;
      case 1:
        rainbowCycle(0); // fast
        break;
      case 2:
        long idx = random(NUM_COLORS);
        wave(strip.Color(mycolors[idx][0],mycolors[idx][1],mycolors[idx][2]), 4, 20); // 4 cycles, wait 20
        break;
    }
  } else {
    breath();
  }
}

// PIR code //////////////////////////////////////////

boolean motionDetected() {
 int sense = digitalRead(PIR_PIN);
 return sense == HIGH;
}

// Photocell code /////////////////////////////////////

boolean isDark() {
  int photocellReading = analogRead(PCELL_PIN);
  return photocellReading < 200;
}

void clearStrip() {
  for (int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
  }
  strip.show();
}

// Ripple code ////////////////////////////////////////

void ripple() {
  if (BG){
    if (currentBg == nextBg) {
      nextBg = random(256);
    } 
    else if (nextBg > currentBg) {
      currentBg++;
    } else {
      currentBg--;
    }
    for(uint16_t l = 0; l < NUM_PIXELS; l++) {
      strip.setPixelColor(l, RippleWheel(currentBg, 0.1));
    }
  } else {
    for(uint16_t l = 0; l < NUM_PIXELS; l++) {
      strip.setPixelColor(l, 0, 0, 0);
    }
  }

  if (step == -1) {
    center = random(NUM_PIXELS);
    color = random(256);
    step = 0;
  }
  
  if (step == 0) {
    strip.setPixelColor(center, RippleWheel(color, 1));
    step ++;
  } 
  else {
    if (step < maxSteps) {
      strip.setPixelColor(wrap(center + step), RippleWheel(color, pow(fadeRate, step)));
      strip.setPixelColor(wrap(center - step), RippleWheel(color, pow(fadeRate, step)));
      if (step > 3) {
        strip.setPixelColor(wrap(center + step - 3), RippleWheel(color, pow(fadeRate, step - 2)));
        strip.setPixelColor(wrap(center - step + 3), RippleWheel(color, pow(fadeRate, step - 2)));
      }
      step ++;
    } 
    else {
      step = -1;
    }
  }
  
  strip.show();
  delay(50);
}

int wrap(int step) {
  if(step < 0) return NUM_PIXELS + step;
  if(step > NUM_PIXELS - 1) return step - NUM_PIXELS;
  return step;
}

// Helper functions ///////////////////////////////////////////

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t RippleWheel(byte WheelPos, float opacity) {
  
  if(WheelPos < 85) {
    return strip.Color((WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color((255 - WheelPos * 3) * opacity, 0, (WheelPos * 3) * opacity);
  } 
  else {
    WheelPos -= 170;
    return strip.Color(0, (WheelPos * 3) * opacity, (255 - WheelPos * 3) * opacity);
  }
}

uint32_t BigWheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}

// Breathing code ////////////////////////////////

const int  CYCLE_MILLISECONDS = 5000; // 5 second breathing cycle.
const uint8_t KEYFRAMES[]  = {
  // Rising
  20, 21, 22, 24, 26, 28, 31, 34, 38, 41, 45, 50, 55, 60, 66, 73, 80, 87, 95,
  103, 112, 121, 131, 141, 151, 161, 172, 182, 192, 202, 211, 220, 228, 236,
  242, 247, 251, 254, 255,

  // Falling
  254, 251, 247, 242, 236, 228, 220, 211, 202, 192, 182, 172, 161, 151, 141,
  131, 121, 112, 103, 95, 87, 80, 73, 66, 60, 55, 50, 45, 41, 38, 34, 31, 28,
  26, 24, 22, 21, 20,
  20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 
};

unsigned long lastBreath = 0.0;
int keyframePointer = 0;

void breath() {
  int numKeyframes = sizeof(KEYFRAMES) - 1;
  float period = CYCLE_MILLISECONDS / numKeyframes;
  unsigned long now = millis();
  
  if ((now - lastBreath) > period) {
    lastBreath = now;

    for (int i = 0; i < strip.numPixels(); i++) {
      uint8_t color = (127 * KEYFRAMES[keyframePointer]) / 256;
      strip.setPixelColor(i, color, 0, 0);
    }
    strip.show();

    // Increment the keyframe pointer.
    if (++keyframePointer > numKeyframes) {
      // Reset to 0 after the last keyframe.
      keyframePointer = 0;
    }   
  }
}

// Sine wave effect //////////////////////////////////

#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    for(int i=0; i<strip.numPixels(); i++) {
      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();
    delay(wait);
  }
}

// Rainbow cycle /////////////////////////////

// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 10; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, BigWheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

