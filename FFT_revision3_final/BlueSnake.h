#include "Arduino.h"

class BlueSnake {
  public:
    BlueSnake(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void BlueSnake::runPattern() {
  uint16_t sine1 = beatsin16(30, 0, NUM_LEDS - 1, 0, 0); // time base and offset
  uint16_t sine2 = beatsin16(60, 0, NUM_LEDS - 1, 0, 0);
  leds[(sine1+sine2)/2] = CHSV(160, 255, 255);
  leds[sine2] = CHSV(0, 255, 255);
  blur1d(leds, NUM_LEDS, 200);
  fadeToBlackBy(leds, NUM_LEDS, 10); // кожного разу буде затемнятись на 10 відсотків
  btn.tick();
  btn2.tick();
  FastLED.show();
}
