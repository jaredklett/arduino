#include <Adafruit_NeoPixel.h>

#define LED_PIN 0
#define RAND_PIN 4

#define NUM_PIXELS 16
#define ARRAY_SIZE 6
#define NOT_ASSIGNED -1
#define BRIGHTER 0
#define DIMMER 1
#define MAX_BRIGHTNESS 192
#define NUM_FAVS 10
#define MAX_SPEED 3

uint8_t favorites[NUM_FAVS][3] = {
  {255, 0, 27}, // pink
  {255, 0, 0}, // red
  {255, 255, 0}, // yellow
  {0, 255, 0}, // green
  {255, 140, 0}, // orange
  {0, 0, 255}, // blue
  {0, 255, 255}, // cyan
  {255, 215, 0}, // gold
  {173, 255, 47}, // yellow/green
  {255, 0, 255} // fuchsia
};

int speeds[ARRAY_SIZE];
uint32_t colors[ARRAY_SIZE];
uint16_t brightnessValues[ARRAY_SIZE];
int pixelpositions[ARRAY_SIZE];
int directions[ARRAY_SIZE];

int activePixels = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);
/*
class Spark {

  public:

  uint32_t currentColor;
  uint32_t targetColor;
  int pixelIndex;

  // Constructor
  Spark(uint32_t color, int index) {
    targetColor = color;
    pixelIndex = index;
    currentColor = 0;
  }

  uint8_t getRed(uint32_t color) {
    return (color >> 16) & 0xFF;
  }

  uint8_t getGreen(uint32_t color) {
    return (color >> 8) & 0xFF;
  }

  uint8_t getBlue(uint32_t color) {
    return color & 0xFF;
  }

  uint32_t dimColor(uint32_t color) {
    uint32_t dimmer = strip.Color(getRed(color) >> 1, getGreen(color) >> 1, getBlue(color) >> 1);
    return dimmer;
  }


} // class Spark
*/
void setup() {
  randomSeed(analogRead(RAND_PIN));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  // init arrays
  for (int i = 0; i < ARRAY_SIZE; i++) {
    pixelpositions[i] = NOT_ASSIGNED;
  }
}

void loop() {
  // 0. Keep two arrays of fixed size: one with brightness values, and one with position values
  // 1. Loop through arrays and update pixels
  // 2. If we find not assigned positions, pick a random pixel to spark
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (pixelpositions[i] == NOT_ASSIGNED) {
      pixelpositions[i] = findRandomPixel();
      int rfcindex = (int)random(NUM_FAVS);
      colors[i] = strip.Color(favorites[rfcindex][0], favorites[rfcindex][1], favorites[rfcindex][2]); // TODO: make different colors?
      brightnessValues[i] = 0;
      directions[i] = BRIGHTER;
      speeds[i] = (int)random(1, MAX_SPEED);
    } else {
      setPixelBrightness(pixelpositions[i], getRed(colors[i]), getGreen(colors[i]), getBlue(colors[i]), brightnessValues[i]);
      if (directions[i] == BRIGHTER) {
        brightnessValues[i] += speeds[i];
        if (brightnessValues[i] >= MAX_BRIGHTNESS) {
          directions[i] = DIMMER;
        }
      } else {
        brightnessValues[i] -= speeds[i];
        // Reset if we are now off
        if (brightnessValues[i] <= 0) {
          strip.setPixelColor(pixelpositions[i], 0);
          pixelpositions[i] = NOT_ASSIGNED;
        }
      }
    }
  }
  strip.show();
  delay(5);
}

int findRandomPixel() {
  int randomPosition = (int)random(NUM_PIXELS);
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (pixelpositions[i] == randomPosition) {
      randomPosition = findRandomPixel();
      break;
    }
  }
  return randomPosition;
}

void setPixelBrightness(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint16_t brght) {
  strip.setPixelColor(i, (brght * r / 255), (brght * g / 255), (brght * b / 255));
}

uint8_t getRed(uint32_t color) {
  return (color >> 16) & 0xFF;
}

uint8_t getGreen(uint32_t color) {
  return (color >> 8) & 0xFF;
}

uint8_t getBlue(uint32_t color) {
  return color & 0xFF;
}


