#include "Arduino.h"

class ColorCorrection {
  public:
    ColorCorrection(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void ColorCorrection::runPattern() {
  
  for (int i = 0; i < NUM_LEDS/3; i++) {
    leds[i] = CRGB::Purple;
  }

  for(int i = NUM_LEDS/3; i < 2*NUM_LEDS/3; i++) {
    leds[i] = CRGB::Yellow;
  }
  
  for(int i = 2*NUM_LEDS/3; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Green;
  }
  btn.tick();
  btn2.tick();
  FastLED.show();
}
