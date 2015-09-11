#include <Adafruit_NeoPixel.h>

#define RAND_PIN A1

#define RING24_PIN 3
#define RING12_PIN 4
#define SINGLE_PIN 5

#define RING24_NUM_PIXELS 24
#define RING12_NUM_PIXELS 12
#define SINGLE_NUM_PIXELS 1

#define RING24_NUM_SPARKS 8
#define RING12_NUM_SPARKS 4

#define BRIGHTER 0
#define DIMMER 1
#define MAX_BRIGHTNESS 222
#define MIN_SPEED 4
#define MAX_SPEED 6
#define INTERVAL 5

#define NUM_PALETTES 3
#define PALETTE_DEPTH 12
#define PALETTE_SWITCH_INTERVAL 30000

// Disorient colors with an accent
uint8_t palette1[][3] = {
  {255, 140, 0}, // orange
  {255, 105, 180}, // BRIGHT pink
  {128, 0, 128} // purple
};

// Ice
uint8_t palette2[][3] = {
  {0, 0, 255}, // blue
  {0, 255, 255}, // cyan
  {128, 128, 128} // white
};

// Traffic light
uint8_t palette3[][3] = {
  {255, 0, 0}, // red
  {255, 255, 0}, // yellow
  {0, 255, 0} // green
};

unsigned long lastUpdate = 0;
unsigned long switchPalette = 0;

int paletteIndex = 0;

// NeoObject class /////////////////////////////////////////////////////////////

class NeoObject : public Adafruit_NeoPixel
{
  public:

  uint32_t palettes[NUM_PALETTES][PALETTE_DEPTH];

  NeoObject(uint16_t pixels, uint8_t pin, uint8_t type)
  :Adafruit_NeoPixel(pixels, pin, type)
  {
    for (int x = 0; x < NUM_PALETTES; x++) {
      for (int y = 0; y < PALETTE_DEPTH; y++) {
        int i = y % 3;
        switch (x) {
          case 0:
            palettes[x][y] = Color(palette1[i][0], palette1[i][1], palette1[i][2]);
            break;
          case 1:
            palettes[x][y] = Color(palette2[i][0], palette2[i][1], palette2[i][2]);
            break;
          case 2:
            palettes[x][y] = Color(palette3[i][0], palette3[i][1], palette3[i][2]);
            break;
        }
      }
    }
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

  void setPixelBrightness(uint16_t i, uint8_t r, uint8_t g, uint8_t b, uint16_t brght) {
    setPixelColor(i, (brght * r / 255), (brght * g / 255), (brght * b / 255));
  }

}; // class NeoObject

// NeoRing24 class /////////////////////////////////////////////////////////////

class NeoRing24 : public NeoObject
{
  public:

  int speeds[RING24_NUM_SPARKS];
  uint32_t colors[RING24_NUM_SPARKS];
  uint16_t brightnessValues[RING24_NUM_SPARKS];
  int pixelpositions[RING24_NUM_SPARKS];
  int directions[RING24_NUM_SPARKS];

  NeoRing24(uint16_t pixels, uint8_t pin, uint8_t type)
  :NeoObject(pixels, pin, type)
  {
    for (int i = 0; i < RING24_NUM_SPARKS; i++) {
      resetSpark(i);
    }
  }

  void updateSparks() {
    for (int i = 0; i < RING24_NUM_SPARKS; i++) {
      setPixelBrightness(pixelpositions[i], getRed(colors[i]), getGreen(colors[i]), getBlue(colors[i]), brightnessValues[i]);
      show();
      if (directions[i] == BRIGHTER) {
        brightnessValues[i] += speeds[i];
        if (brightnessValues[i] >= MAX_BRIGHTNESS) {
          directions[i] = DIMMER;
        }
      } else {
        brightnessValues[i] -= speeds[i];
        // Reset if we are now off
        if (brightnessValues[i] <= 0) {
          setPixelColor(pixelpositions[i], 0);
          show();
          resetSpark(i);
        }
      }
    }
  }

  void resetSpark(int i) {
    int rfcindex = (int)random(PALETTE_DEPTH);
    colors[i] = palettes[paletteIndex][rfcindex];
    pixelpositions[i] = findRandomPixel();
    brightnessValues[i] = 0;
    directions[i] = BRIGHTER;
    speeds[i] = (int)random(1, MAX_SPEED);
  }

  int findRandomPixel() {
    int randomPosition = (int)random(RING24_NUM_PIXELS);
    for (int i = 0; i < RING24_NUM_SPARKS; i++) {
      if (pixelpositions[i] == randomPosition) {
        randomPosition = findRandomPixel();
        break;
      }
    }
    return randomPosition;
  }

  void clearAll() {
    for (int i = 0; i < RING24_NUM_SPARKS; i++) {
      resetSpark(i);
    }
    clear();
  }

}; // class NeoRing24

// NeoRing12 class /////////////////////////////////////////////////////////////

class NeoRing12 : public NeoObject
{
  public:

  int speeds[RING12_NUM_SPARKS];
  uint32_t colors[RING12_NUM_SPARKS];
  uint16_t brightnessValues[RING12_NUM_SPARKS];
  int pixelpositions[RING12_NUM_SPARKS];
  int directions[RING12_NUM_SPARKS];

  NeoRing12(uint16_t pixels, uint8_t pin, uint8_t type)
  :NeoObject(pixels, pin, type)
  {
    for (int i = 0; i < RING12_NUM_SPARKS; i++) {
      resetSpark(i);
    }
  }

  void updateSparks() {
    for (int i = 0; i < RING12_NUM_SPARKS; i++) {
      setPixelBrightness(pixelpositions[i], getRed(colors[i]), getGreen(colors[i]), getBlue(colors[i]), brightnessValues[i]);
      show();
      if (directions[i] == BRIGHTER) {
        brightnessValues[i] += speeds[i];
        if (brightnessValues[i] >= MAX_BRIGHTNESS) {
          directions[i] = DIMMER;
        }
      } else {
        brightnessValues[i] -= speeds[i];
        // Reset if we are now off
        if (brightnessValues[i] <= 0) {
          setPixelColor(pixelpositions[i], 0);
          show();
          resetSpark(i);
        }
      }
    }
  }

  void resetSpark(int i) {
    int rfcindex = (int)random(PALETTE_DEPTH);
    colors[i] = palettes[paletteIndex][rfcindex];
    pixelpositions[i] = findRandomPixel();
    brightnessValues[i] = 0;
    directions[i] = BRIGHTER;
    speeds[i] = (int)random(MIN_SPEED, MAX_SPEED);
  }

  int findRandomPixel() {
    int randomPosition = (int)random(RING12_NUM_PIXELS);
    for (int i = 0; i < RING12_NUM_SPARKS; i++) {
      if (pixelpositions[i] == randomPosition) {
        randomPosition = findRandomPixel();
        break;
      }
    }
    return randomPosition;
  }

  void clearAll() {
    for (int i = 0; i < RING12_NUM_SPARKS; i++) {
      resetSpark(i);
    }
    clear();
  }

}; // class NeoRing12

// NeoSingle class //////////////////////////////////////////////////////////////

class NeoSingle : public NeoObject
{
  public:

  int theSpeed;
  uint32_t theColor;
  uint16_t brightnessValue;
  int pixelposition;
  int theDirection;

  NeoSingle(uint16_t pixels, uint8_t pin, uint8_t type)
  :NeoObject(pixels, pin, type)
  {
    resetSpark();
  }

  void updateSpark() {
    setPixelBrightness(pixelposition, getRed(theColor), getGreen(theColor), getBlue(theColor), brightnessValue);
    show();
    if (theDirection == BRIGHTER) {
      brightnessValue += theSpeed;
      if (brightnessValue >= MAX_BRIGHTNESS) {
        theDirection = DIMMER;
      }
    } else {
      brightnessValue -= theSpeed;
      // Reset if we are now off
      if (brightnessValue <= 0) {
        setPixelColor(pixelposition, 0);
        show();
        resetSpark();
      }
    }
  }

  void resetSpark() {
    int rfcindex = (int)random(PALETTE_DEPTH);
    theColor = palettes[paletteIndex][rfcindex];
    pixelposition = 0;
    brightnessValue = 0;
    theDirection = BRIGHTER;
    theSpeed = (int)random(MIN_SPEED, MAX_SPEED);
  }

  void clearAll() {
    resetSpark();
    clear();
  }

}; // class NeoSingle

// Arduino methods //////////////////////////////////////////////////////////////

NeoRing24 ring24 = NeoRing24(RING24_NUM_PIXELS, RING24_PIN, NEO_GRB + NEO_KHZ800);
NeoRing12 ring12 = NeoRing12(RING12_NUM_PIXELS, RING12_PIN, NEO_GRB + NEO_KHZ800);
NeoSingle single = NeoSingle(SINGLE_NUM_PIXELS, SINGLE_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  randomSeed(analogRead(RAND_PIN));
  ring24.begin();
  ring12.begin();
  single.begin();
  ring24.show();
  ring12.show();
  single.show();
}

void loop() {
  if ((millis() - lastUpdate) > INTERVAL) {
    lastUpdate = millis();
    ring24.updateSparks();
    ring12.updateSparks();
    single.updateSpark();
  }
  if ((millis() - switchPalette) > PALETTE_SWITCH_INTERVAL) {
    switchPalette = millis();
    paletteIndex++;
    if (paletteIndex >= NUM_PALETTES) {
      paletteIndex = 0;
    }
    ring24.clearAll();
    ring12.clearAll();
    single.clearAll();
  }
}

