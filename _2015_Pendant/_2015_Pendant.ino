
#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, SPARKLE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

class NeoPatterns : public Adafruit_NeoPixel {

    public:

    // Member Variables:  
    pattern ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    
    void (*OnComplete)();  // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }

    // Update the pattern
    void Update() {
        if((millis() - lastUpdate) > Interval) { // time to update
            lastUpdate = millis();
            switch(ActivePattern) {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case SPARKLE:
                    SparkleUpdate();
                    break;
                default:
                    break;
            }
        }
    }

    // Increment the Index and reset at the end
    void Increment() {
        if (Direction == FORWARD) {
           Index++;
           if (Index >= TotalSteps) {
                Index = 0;
                if (OnComplete != NULL) {
                    OnComplete(); // call the comlpetion callback
                }
            }
        } else { // Direction == REVERSE
            --Index;
            if (Index <= 0) {
                Index = TotalSteps-1;
                if (OnComplete != NULL) {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
    }

    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD) {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate() {
        for(int i = 0; i < numPixels(); i++) {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
    }

    void Sparkle(uint8_t interval, direction dir = FORWARD) {
        ActivePattern = SPARKLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }

    void SparkleUpdate() {
        
    }
    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color) {
        return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color) {
        return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color) {
        return color & 0xFF;
    }

    // Return color, dimmed by 75% (used by scanner)
    uint32_t DimColor(uint32_t color) {
        uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
        return dimColor;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos) {
        WheelPos = 255 - WheelPos;
        if(WheelPos < 85) {
            return Color(255 - WheelPos * 3, 0, WheelPos * 3);
        } else if(WheelPos < 170) {
            WheelPos -= 85;
            return Color(0, WheelPos * 3, 255 - WheelPos * 3);
        } else {
            WheelPos -= 170;
            return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
        }
    }

    // Reverse direction of the pattern
    void Reverse() {
        if (Direction == FORWARD) {
            Direction = REVERSE;
            Index = TotalSteps-1;
        } else {
            Direction = FORWARD;
            Index = 0;
        }
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color) {
        for (int i = 0; i < numPixels(); i++) {
            setPixelColor(i, color);
        }
        show();
    }

}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
