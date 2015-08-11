#include <Adafruit_NeoPixel.h>

#define LED_PIN 0
#define RAND_PIN 4

#define NUM_PIXELS 16
#define ARRAY_SIZE 6
#define NOT_ASSIGNED -1
#define BRIGHTER 0
#define DIMMER 1
#define MAX_BRIGHTNESS 222
#define NUM_FAVS 7
#define MAX_SPEED 4
#define INTERVAL 5

uint8_t favorites[NUM_FAVS][3] = {
  {255, 0, 0}, // red
  {255, 140, 0}, // orange
  {255, 255, 0}, // yellow
  {0, 255, 0}, // green
  {0, 0, 255}, // blue
  {0, 255, 255}, // cyan
  {255, 0, 255} // fuchsia
};

int speeds[ARRAY_SIZE];
uint32_t colors[ARRAY_SIZE];
uint16_t brightnessValues[ARRAY_SIZE];
int pixelpositions[ARRAY_SIZE];
int directions[ARRAY_SIZE];
unsigned long lastUpdate;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

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
  if ((millis() - lastUpdate) > INTERVAL) {
    lastUpdate = millis();
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
  }
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


