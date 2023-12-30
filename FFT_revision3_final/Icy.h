#include "Arduino.h"

class Icy {
  public:
    Icy(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void Icy::runPattern() {
    EVERY_N_MILLISECONDS(50) {
    
    leds[0] = CHSV(160, random8(), random8(100, 255));
    
    for (int i = NUM_LEDS - 1; i > 0; i--) {
      leds[i] = leds[i - 1];
    }
  }
  btn.tick();
  btn2.tick();
  FastLED.show();
}
