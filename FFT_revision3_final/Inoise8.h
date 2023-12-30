#include "Arduino.h"

class Inoise8 {
  public:
    Inoise8(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void Inoise8::runPattern() {
  for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t color = inoise8(i * 10, 2);
      leds[i] = CHSV(color, 255, 255);
  }
  btn.tick();
  btn2.tick();
  FastLED.show();
}
