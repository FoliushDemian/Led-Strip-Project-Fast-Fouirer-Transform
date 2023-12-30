#include "Arduino.h"

class RainbowBeat {
  public:
    RainbowBeat(){};
    void runPattern();
  private:
};

void RainbowBeat::runPattern() {
  
  uint16_t beatA = beatsin16(30, 0, 255); // 30 is how many waves per minute function is going, 0 is low value, 255 is max value
  uint16_t beatB = beatsin16(20, 0, 255);
  fill_rainbow(leds, NUM_LEDS, (beatA+beatB)/2, 15); // (beatA+beatB)/2 is initial shade color, 15 This argument determines how different the hue of 
                                                                                             // each subsequent LED will be from the previous one.     
  btn.tick();
  btn2.tick();
  FastLED.show();
}
