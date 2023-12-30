#include "Arduino.h"

class MonoColor {
  public:
    MonoColor(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void MonoColor::runPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Blue;
  }
  btn.tick();
  btn2.tick();
  FastLED.show();
}
