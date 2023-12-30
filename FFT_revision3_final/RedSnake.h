#include "Arduino.h"

class RedSnake {
  public:
    RedSnake(){
    // Blank constructor 
    };
    void runPattern();
  private:
};


void RedSnake::runPattern() {
   for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Blue;
   }
  btn.tick();
  btn2.tick();
  FastLED.show();
  delay(20);
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;
   }
  btn.tick();
  btn2.tick();
  FastLED.show();
  delay(20);
  for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::White;
   }
  btn.tick();
  btn2.tick();
  FastLED.show();
  delay(20);
}
