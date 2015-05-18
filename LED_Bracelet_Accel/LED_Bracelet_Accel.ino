#include <TinyWireM.h>
#include <Adafruit_TinyLSM303.h>
#include <Adafruit_NeoPixel.h>

#define MOVE_THRESHOLD 6000
#define PIN 1
#define NUM_PIXELS 20

int32_t storedVector = 0,
         newVector = 0;
//uint32_t color = 0,
uint32_t color_timer = 0,
         action_step_timer = 0,
         last_shacke = 0,
         sparkel_timer = 0,
         brightness_decay_timer = 0;
uint16_t color_idx = 0,
         curr_color_interval = 1000,
         curr_brightness_decay_interval = 30,
         party_interval = 1000,
         sparkel_interval = 5000;
uint8_t  r, // global red
         g, // global green
         b, // global blue
         spectrum_part = 0,
         curr_action = 0,
         curr_color_granularity = 1,
         nr_shackes = 0;
         
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_TinyLSM303 lsm;

void setup() {
  strip.begin();
  strip.setBrightness(80);
  nextColor();
  strip.show();
  if (!lsm.begin())
  {
    while (1);
  }
}

void loop() {
  lsm.read();
  newVector = lsm.accelData.x*lsm.accelData.x;
  newVector += lsm.accelData.y*lsm.accelData.y;
  newVector += lsm.accelData.z*lsm.accelData.z;
  newVector = sqrt(newVector);
  curr_color_granularity = 1;

  if (abs(newVector - storedVector) > MOVE_THRESHOLD) {
    curr_color_granularity = 20;
    nextColor();
    last_shacke = millis();
    nr_shackes++;
  }
  if((millis() - last_shacke) >  party_interval){
    nr_shackes = 0;
  } 
  
  storedVector = newVector;
  /*
  if(nr_shackes > 8){
    rainbowCycle(2, 10);
     nr_shackes = 0;
  }
  */
  if ((millis() - color_timer) > curr_color_interval)
  {
    nextColor();
    color_timer = millis();
  }
  
  scanner(30); // 30 = slow, 15 = fast    
}

void nextColor ()
{
  switch (spectrum_part)
  {
    case 0 :  // spectral wipe from red to blue
    {
      r = 255-color_idx;
      g = color_idx;
      b = 0;
      color_idx += curr_color_granularity;
      if (color_idx > 255) 
      {
          spectrum_part = 1;
          color_idx = 0;
      }
      break;
    }
    case 1 :  // spectral wipe from blue to green
    {
      r = 0;
      g = 255-color_idx;
      b = color_idx;
      color_idx += curr_color_granularity;
      if (color_idx > 255) 
      {
          spectrum_part = 2;
          color_idx = 0;
      }
      break;
    }
    case 2 :  // spectral wipe from green to red
    {
      r = color_idx;
      g = 0;
      b = 255-color_idx;
      color_idx += curr_color_granularity;
      if (color_idx > 255) 
      {
          spectrum_part = 0;
          color_idx = 0;
      }
      break;
    }
    
  }
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t wait) {
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 8); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    delay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}
/*
void setColor(uint32_t color){
  for (int i = 0; i < NUM_PIXELS; i++) strip.setPixelColor(i,color);
  strip.show();
}
*/
/*
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(byte num_cycles, uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*num_cycles; j++) {
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
*/
