#include "Arduino.h"
#include "AudioAnalysis.h"
  
class MovingDot {
  public:
    MovingDot(){
      // Blank constructor  
    };
    void runPattern();
  private:
};

byte *Wheel(byte WheelPosition) {
  static byte c[3];
 
  if(WheelPosition < 85) {
   c[0] = WheelPosition * 3;
   c[1] = 255 - WheelPosition * 3;
   c[2] = 0;
  }
  else if(WheelPosition < 170) {
   WheelPosition -= 85;
   c[0] = 255 - WheelPosition * 3;
   c[1] = 0;
   c[2] = WheelPosition * 3;
  }
  else {
   WheelPosition -= 170;
   c[0] = 0;
   c[1] = WheelPosition * 3;
   c[2] = 255 - WheelPosition * 3;
  }
  
  btn.tick();
  btn2.tick();

  return c;
}

void MovingDot::runPattern() {
  
//  byte *c;
//  uint16_t i, j;
//
//  btn.tick();
//  btn2.tick();
//  for(j=0; j < 256; j++) {
//    btn.tick();
//    btn2.tick();
//    for(i=0; i < NUM_LEDS; i++) {
//      btn.tick();
//      btn2.tick();
//      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
//      leds[NUM_LEDS - 1 - i] = CRGB(*c, *(c+1), *(c+2));  
//    }
//    btn.tick();
//    btn2.tick();
//    FastLED.show();
//    delay(20);
//  }


  for (int i = 0; i < 43; i++) {
    leds[i] = CRGB(128, 0, 128);;
    leds[i+42] = CRGB(0, 0, 255);
    leds[i+85] = CRGB(0, 191, 255);
    leds[i+128] = CRGB::Green;
    leds[i+171] = CRGB(255, 255, 0); 
    leds[i+214] = CRGB(255, 128, 0); 
    leds[i+257] = CRGB::Red;
  }
  btn.tick();
  btn2.tick();
  FastLED.show();
}
