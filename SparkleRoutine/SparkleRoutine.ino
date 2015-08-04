#include <Adafruit_NeoPixel.h>

#define LED_PIN 1
#define RAND_PIN A10

#define NUM_PIXELS 16
#define ARRAY_SIZE 10

uint32_t colors[ARRAY_SIZE];
int pixelpositions[ARRAY_SIZE];

int activePixels = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  randomSeed(analogRead(RAND_PIN));
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  // 0. Keep two arrays of fixed size: one with brightness values, and one with position values
  // 1. Pick a random pixel to spark
  // 2. 
}
