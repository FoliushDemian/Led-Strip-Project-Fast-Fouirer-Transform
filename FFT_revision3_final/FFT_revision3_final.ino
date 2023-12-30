#include "AudioInI2S.h"
#include <OneButton.h>

#define SAMPLE_SIZE 256  // Buffer size of read samples
#define SAMPLE_RATE 20000 // Audio Sample Rate

#define BAND_SIZE 8 
#include "ESP32FFT.h"

EspFFT<float> *_FFT = nullptr;

#include "AudioAnalysis.h"
AudioAnalysis audioInfo;

#define MIC_BCK_PIN 32             // Clock pin from the mic.
#define MIC_WS_PIN 25             // WS pin from the mic.
#define MIC_DATA_PIN 33     
#define BTN_PIN   16
#define MOVE_DETECTION_PIN   26   // Data pin from the mic.

uint8_t patternCounter = 0; // for change mode of RGB
//bool isRunning = false;

// Push button connected between pin 16 and GND (no resistor required)
OneButton btn = OneButton(BTN_PIN, true, true);

//#define BTN_PIN2   17            // For change move detection and microphone

// Pins connectors seven segment display
#define PIN_A 27
#define PIN_B 14
#define PIN_C 18
#define PIN_D 19
#define PIN_E 21
#define PIN_F 22
#define PIN_G 23
#define PIN_DP 5


AudioInI2S mic(MIC_BCK_PIN, MIC_WS_PIN, MIC_DATA_PIN); // defaults to RIGHT channel.

int32_t samples[SAMPLE_SIZE]; // I2S sample data is stored here

#include "FastLED.h"
//#define NUM_LEDS 300
#define LED_PIN 12
int MAX_BRIGHTNESS = 100; 
#define FRAME_RATE 30
//CRGB leds[NUM_LEDS];
unsigned long nextFrame = 0;
unsigned long tick = 0;

float _weighingFactors[SAMPLE_SIZE];

#include "MovingDot.h"
#include "RainbowBeat.h"
#include "RedWhiteBlue.h"
#include "MonoColor.h"
#include "RedSnake.h"
#include "BlueSnake.h"
#include "Inoise8.h"
#include "Icy.h"
#include "ColorCorrection.h"

double peaking;


void setup()
{
  Serial.begin(115200);
  mic.begin(SAMPLE_SIZE, SAMPLE_RATE); 

  // audio analysis setup
  audioInfo.setNoiseFloor(50);       // sets the noise floor
  audioInfo.normalize(true, 0, 255); // normalize all values to range provided. 

  audioInfo.autoLevel(AudioAnalysis::EXPONENTIAL_FALLOFF, 1, 255, 5000); // встановити швидкість автоматичного зниження рівня
  audioInfo.bandPeakFalloff(AudioAnalysis::EXPONENTIAL_FALLOFF, 1);     // встановити швидкість спаду піку діапазону
  audioInfo.vuPeakFalloff(AudioAnalysis::ACCELERATE_FALLOFF, 1);        // встановити швидкість пікового падіння одиниці об’єму


  audioInfo.setEqualizerLevels(0.75, 1.25, 1.5); // set the equlizer offsets

  pinMode(MOVE_DETECTION_PIN, INPUT);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalPixelString);

  btn.attachClick(nextPattern);
  btn.attachLongPressStart(myDoubleClickHandlerUp);
  btn2.attachClick(nextPattern2);
  btn2.attachDuringLongPress(myDoubleClickHandlerDown);
  
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);
  pinMode(PIN_E, OUTPUT);
  pinMode(PIN_F, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_DP, OUTPUT);
}

void loop()
{
  FastLED.setBrightness(MAX_BRIGHTNESS);
  //Serial.println("patternCounter2");
  //Serial.println(patternCounter2);
  switch (patternCounter2) {
    case 0:
        //Serial.println("microphone mode");
        digitalWrite(PIN_A, 1);
        digitalWrite(PIN_B, 1);
        digitalWrite(PIN_C, 1);
        digitalWrite(PIN_D, 1);
        digitalWrite(PIN_E, 1);
        digitalWrite(PIN_F, 1);
        digitalWrite(PIN_G, 0);
        digitalWrite(PIN_DP, 1);
        runMicrophone();
      break;
    case 1:
      int pinValue = digitalRead(MOVE_DETECTION_PIN);
      if (pinValue == HIGH && patternCounter2 == 1)
      {
        Serial.println("HIGH signal");
        switch (patternCounter) {
          case 0:
            Serial.println("case 0 inside"); 
            digitalWrite(PIN_A, 0);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 0);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_F, 0);
            digitalWrite(PIN_G, 0);
            digitalWrite(PIN_DP, 1);
            runMovingDot();  
            Serial.println("case 0 escape");  
            break;
          case 1:
            Serial.println("case 1 inside"); 
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 0);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_F, 0);
            digitalWrite(PIN_E, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_DP, 1);
            runRainbowBeat();
            Serial.println("case 1 escape"); 
            break;
          case 2:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_F, 0);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_DP, 1);
            runRedWhiteBlue();
            break;
          case 3:
            digitalWrite(PIN_A, 0);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 0);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_F, 1);
            digitalWrite(PIN_DP, 1);
            runColorCorrection();
            break;
          case 4:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 0);
            digitalWrite(PIN_F, 1);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_DP, 1);
            runRedSnake();
            break;
          case 5:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 0);
            digitalWrite(PIN_F, 1);
            digitalWrite(PIN_E, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_DP, 1);
            runBlueSnake();
            break;
          case 6:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_F, 0);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_D, 0);
            digitalWrite(PIN_F, 0);
            digitalWrite(PIN_G, 0);
            digitalWrite(PIN_DP, 1);
            runInoise8();
            break;
          case 7:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_E, 1);
            digitalWrite(PIN_F, 1);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_DP, 1); 
            runIcy();
            break;
          case 8:
            digitalWrite(PIN_A, 1);
            digitalWrite(PIN_B, 1);
            digitalWrite(PIN_C, 1);
            digitalWrite(PIN_D, 1);
            digitalWrite(PIN_E, 0);
            digitalWrite(PIN_G, 1);
            digitalWrite(PIN_F, 1);
            digitalWrite(PIN_DP, 1);
            runMonoColor();
            break;
        }
      } else {
        Serial.println("LOW signal");
        FastLED.clear();
        FastLED.show();
      }
      delay(500);   
      break;
  }
}

void processSamples()
{
  mic.read(samples); // Stores the current I2S port buffer into samples.
  _FFT = new EspFFT<float>(audioInfo.getReal(), audioInfo.getImaginary(), SAMPLE_SIZE, SAMPLE_RATE, _weighingFactors);
  audioInfo.computeFFT(samples, SAMPLE_SIZE, SAMPLE_RATE);
  //audioInfo.computeFrequencies(BAND_SIZE);  // BAND_SIZE powers of 2 up to 64, defaults to 8
}


//void renderBeatRainbow()
//{
//  float *bands = audioInfo.getBands();
//  float *peaks = audioInfo.getPeaks();
//  int peakBandIndex = audioInfo.getBandMaxIndex();
//  int peakBandValue = audioInfo.getBand(peakBandIndex);
//  static int beatCount = 0;
//
//  bool beatDetected = false;
//  bool clapsDetected = false;
//  // beat detection
//  if (peaks[0] == bands[0] && peaks[0] > 0) // new peak for bass must be a beat
//  {
//    beatCount++;
//    beatDetected = true;
//  }
//  if (peakBandIndex >= BAND_SIZE / 2 && peakBandValue > 0)
//  {
//    clapsDetected = true;
//  }
//
//  for (int i = 0; i < NUM_LEDS; i++)
//  {
//    leds[i] = blend(leds[i], CRGB::Green, 255); // fade to violet over time CRGB(0, 0, 0)
//
//    // bass/beat = rainbow
//    if (beatDetected)
//    {
//      if (random(0, 10 - ((float)peaks[1] / (float)255 * 10.0)) == 0)
//      {
//        leds[i] = CHSV((beatCount * 10) % 255, 255, 255);
//      }
//    }
//
//    // claps/highs = white twinkles
//    if (clapsDetected)
//    {
//      if (random(0, 40 - ((float)peakBandIndex / (float)BAND_SIZE * 10.0)) == 0)
//      {
//        leds[i] = CRGB(255, 255, 255);
//      }
//    }
//  }
//  btn2.tick();
//  FastLED.show();
//}


// second
void renderBeatRainbow()
{
//  float *bands = audioInfo.getBands();
//  float *peaks = audioInfo.getPeaks();
//  int peakBandIndex = audioInfo.getBandMaxIndex();
//  int peakBandValue = audioInfo.getBand(peakBandIndex); 
//
//  static int beatCount = 0;
//
//  bool beatDetected = false;
//  bool clapsDetected = false;
//  // beat detection
//  if (peaks[0] == bands[0] && peaks[0] > 0) // new peak for bass must be a beat
//  {
//    beatCount++;
//    beatDetected = true;
//  }
//  if (peakBandIndex >= BAND_SIZE / 2 && peakBandValue > 0)
//  {
//    clapsDetected = true;
//  }
//
//  for (int i = 0; i < NUM_LEDS; i++)
//  {
//    leds[i] = blend(leds[i], CRGB::Purple, 10); // Плавна зміна в фіолетовий колір
//
//    // bass/beat = rainbow
//    if (beatDetected)
//    {
//      int hue = (beatCount * 10) % 255; // Обчислення кольору на основі beatCount
//      leds[i] = CHSV(hue, 255, 255);
//    }
//
//    // claps/highs = white twinkles
//    if (clapsDetected)
//    {
//      if (random(0, 40 - ((float)peakBandIndex / (float)BAND_SIZE * 10.0)) == 0)
//      {
//        leds[i] = CRGB(255, 255, 255);
//      }
//    }
//  }


  //double peaking = _FFT->majorPeak(); 
  //Serial.println("majorPeak:");
  //Serial.println(peaking);

  //Serial.println("getVolumeUnitPeak:");
  //float volumePeak = audioInfo.getVolumeUnitPeak();
  //Serial.println(volumePeak);
  //FastLED.setBrightness(volumePeak);
  
  if (digitalRead(BTN_PIN2) == LOW) { 
      Serial.println("final 1");
      isRunning2 = false;
      nextPattern2();
    }
//  if (peaking <= 500 && isRunning2){                            // FINAL REVIEW!
//     FastLED.clear();
//     btn2.tick();
//     for (int i = 0; i < NUM_LEDS; i++) { // Purple
//      leds[i] = CRGB(128, 0, 128);
//    }
//    
//  } 
//  else if(500 < peaking && peaking <= 800 && isRunning2){
//     FastLED.clear();
//     btn2.tick();
////    for (int i = NUM_LEDS/2,j = NUM_LEDS/2+1 ; i >= 0, j < NUM_LEDS; i--, j++) { // Blue
////      //FastLED.clear();
////      leds[i] = CRGB(0, 0, peaking*0.256);
////      leds[j] = CRGB(0, 0, peaking*0.256);
////      FastLED.show();
////    }
//    
//    for (int i = 0; i < NUM_LEDS; i++) { // Purple
//      double val = map(i, 0, NUM_LEDS-1, 128, 255);
//      leds[i] = CRGB(0, 0, val);
//    }
//    btn2.tick();
//  }
//  else if(800 < peaking && peaking <= 1200 && isRunning2) {
//    FastLED.clear();
//    btn2.tick();
//    for (int i = 0; i < NUM_LEDS; i++) { // light blue
//      leds[i] = CRGB(0, 191, 255);    
//    }
//  } 
//  else if ( 1200 < peaking && peaking <= 2000){
//     FastLED.clear();
//     btn2.tick();
//     for (int i = 0; i < NUM_LEDS; i++) { // Green
//     leds[i] = CRGB(0, peaking * 0.11, 0);   
//    }
//  } 
//  else if(2000 < peaking && peaking <= 3000){
//    FastLED.clear();
//    btn2.tick();
//    for (int i = 0; i < NUM_LEDS; i++) {
//      Serial.println("Yellow");
//      leds[i] = CRGB(peaking * 0.085, peaking * 0.085, 0);    
//    }
//  }
//  else if(3000 < peaking  && peaking <= 4000) {
//    FastLED.clear();
//    btn2.tick();
//    for (int i = 0; i < NUM_LEDS; i++) {
//      Serial.println("light red");
//      leds[i] = CRGB(255, peaking * 0.032, 0);    
//    }
//  } 
//  else if(4000 < peaking) {
//    FastLED.clear();
//    btn2.tick();
//    for (int i = 0; i < NUM_LEDS; i++) {
//      Serial.println("RED");
//      leds[i] = CRGB(128, 0, 0);     
//    }
//  }


//  for (int i = NUM_LEDS/2,j = NUM_LEDS/2+1 ; i >= 0, j < NUM_LEDS; i--, j++) {
//    if (peaking <= 500 && isRunning2){
//      btn2.tick();  
//      leds[i] = CRGB(128, 0, 128); 
//      leds[j] = CRGB(128, 0, 128); 
//      FastLED.show();  
//    }
//    else if(500 < peaking && peaking <= 800 && isRunning2){
//      btn2.tick();    
//      leds[i] = CRGB(0, 0, peaking*0.256); 
//      leds[j] = CRGB(128, 0, peaking*0.256); 
//      FastLED.show();   
//    }
//    else if(800 < peaking && peaking <= 1200 && isRunning2) {
//      btn2.tick(); 
//      leds[i] = CRGB(0, 191, 255);
//      leds[j] = CRGB(128, 191, 255); 
//      FastLED.show(); 
//    } 
//    else if ( 1200 < peaking && peaking <= 2000){
//      btn2.tick(); 
//      leds[i] = CRGB(0, peaking * 0.11, 0); 
//      leds[j] = CRGB(0, peaking * 0.11, 0);
//      FastLED.show();
//    }
//    else if(2000 < peaking && peaking <= 3000){
//      btn2.tick(); 
//      leds[i] = CRGB(peaking * 0.085, peaking * 0.085, 0);
//      leds[j] = CRGB(peaking * 0.085, peaking * 0.085, 0);
//      FastLED.show();       
//    }
//    else if(3000 < peaking  && peaking <= 4000) {
//      btn2.tick(); 
//      leds[i] = CRGB(255, peaking * 0.032, 0); 
//      leds[j] = CRGB(255, peaking * 0.032, 0);
//      FastLED.show();   
//    }
//    else if(4000 < peaking) {
//       btn2.tick(); 
//       leds[i] = CRGB(128, 0, 0);  
//       leds[j] = CRGB(128, 0, 0);
//       FastLED.show();
//    }
//    
//    if (digitalRead(BTN_PIN2) == LOW) { 
//      Serial.println("final 1");
//      isRunning2 = false;
//      nextPattern2();
//    }
//  }
//
//    if (digitalRead(BTN_PIN2) == LOW) { 
//      Serial.println("final 1");
//      isRunning2 = false;
//      nextPattern2();
//    }


  float *bands = audioInfo.getBands(); // gets the last bands calculated from computeFrequencies()
  int vuMeter = audioInfo.getVolumeUnit(); // gets the last volume unit calculated from computeFrequencies()
  float *peaks = audioInfo.getPeaks(); // gets the last peaks calculated from computeFrequencies()
  int vuMeterPeak = audioInfo.getVolumeUnitPeak(); // gets the last volume unit peak calculated from computeFrequencies()
  int vuMeterPeakMax = audioInfo.getVolumeUnitPeakMax(); // value of the highest value volume unit

  FastLED.setBrightness(vuMeterPeak); 
  Serial.println(vuMeterPeak);

  for(int j = 0; j < 38; j++) {
    leds[j] = CHSV(0 * (200 / BAND_SIZE), 255, (int)peaks[0]);
    leds[j+37] = CHSV(1 * (200 / BAND_SIZE), 255, (int)peaks[1]);
    leds[j+74] = CHSV(2 * (200 / BAND_SIZE), 255, (int)peaks[2]);
    leds[j+111] = CHSV(3 * (200 / BAND_SIZE), 255, (int)peaks[3]);
    leds[j+148] = CHSV(4 * (200 / BAND_SIZE), 255, (int)peaks[4]);
    leds[j+185] = CHSV(5 * (200 / BAND_SIZE), 255, (int)peaks[5]);
    leds[j+222] = CHSV(6 * (200 / BAND_SIZE), 255, (int)peaks[6]);
    leds[j+259] = CHSV(7 * (200 / BAND_SIZE), 255, (int)peaks[7]);
  }

//  for(int i = 0; i < BAND_SIZE; i++) {
//    Serial.println(bands[i]);
//  }
//  Serial.println("bands end");
//  for(int i = 0; i < BAND_SIZE; i++) {
//    Serial.println(peaks[i]);
//  }
//  Serial.println("peaks end");
//    for(int j = 0; j < 38; j++) {
//    leds[j] = CHSV(0 * (200 / BAND_SIZE), 255, (int)bands[0]);
//    leds[j+37] = CHSV(1 * (200 / BAND_SIZE), 255, (int)bands[1]);
//    leds[j+74] = CHSV(2 * (200 / BAND_SIZE), 255, (int)bands[2]);
//    leds[j+111] = CHSV(3 * (200 / BAND_SIZE), 255, (int)bands[3]);
//    leds[j+148] = CHSV(4 * (200 / BAND_SIZE), 255, (int)bands[4]);
//    leds[j+185] = CHSV(5 * (200 / BAND_SIZE), 255, (int)bands[5]);
//    leds[j+222] = CHSV(6 * (200 / BAND_SIZE), 255, (int)bands[6]);
//    leds[j+259] = CHSV(7 * (200 / BAND_SIZE), 255, (int)bands[7]);
//  }

}

void nextPattern() {
  isRunning = false;
  patternCounter = (patternCounter + 1) % 9;
}

void runMovingDot(){
  isRunning = true;
  isRunning2 = false;
  MovingDot movingDot = MovingDot();
  Serial.println("runMovingDot() is started");
  delay(300);
  while(isRunning) movingDot.runPattern();
  Serial.println("runMovingDot() is ended");
}

void runRainbowBeat(){
  isRunning = true;
  isRunning2 = false;
  RainbowBeat rainbowBeat = RainbowBeat();
  delay(300);
  while(isRunning) rainbowBeat.runPattern();
}

void runRedWhiteBlue(){
  isRunning = true;
  isRunning2 = false;
  RedWhiteBlue redWhiteBlue = RedWhiteBlue();
  delay(300);
  while(isRunning) redWhiteBlue.runPattern();
}

void runMonoColor() {
  isRunning = true;
  isRunning2 = false;
  MonoColor monoColor = MonoColor();
  delay(300);
  while(isRunning) monoColor.runPattern();
}

void runRedSnake() {
  isRunning = true;
  isRunning2 = false;
  RedSnake redSnake = RedSnake();
  delay(300);
  while(isRunning) redSnake.runPattern();
}

void runBlueSnake() {
  isRunning = true;
  isRunning2 = false;
  BlueSnake blueSnake = BlueSnake();
  delay(300);
  while(isRunning) blueSnake.runPattern();
}

void runInoise8() {
  isRunning = true;
  isRunning2 = false;
  Inoise8 noise8 = Inoise8();
  delay(300);
  while(isRunning) noise8.runPattern();
}

void runIcy() {
  isRunning = true;
  isRunning2 = false;
  Icy icy = Icy();
  delay(300);
  while(isRunning) icy.runPattern();
}

void runColorCorrection() {
  isRunning = true;
  isRunning2 = false;
  ColorCorrection colorCorrection = ColorCorrection();
  delay(300);
  while(isRunning) colorCorrection.runPattern();
}

//void nextPattern2() {
//  isRunning = false;
//  //isRunning2 = true;
//  patternCounter2 = (patternCounter2 + 1) % 2;
//  //Serial.println("calling nextPattern2()");
//  
//}

void runMicrophone() {
  isRunning2 = true;
  while(isRunning2) {
    if (nextFrame > millis())
      {
        return;
      }
      // enforce a predictable frame rate
      nextFrame = millis() + (1000 / FRAME_RATE);
      tick++;

      processSamples(); // does all the reading and frequency calculations

      /* RENDER MODES */
      //renderBeatRainbow(); 
  }
}

void myDoubleClickHandlerDown() {
    for(MAX_BRIGHTNESS; MAX_BRIGHTNESS >= 10; MAX_BRIGHTNESS =  MAX_BRIGHTNESS - 10) {
     FastLED.setBrightness(MAX_BRIGHTNESS);
     Serial.println(MAX_BRIGHTNESS);
     Serial.println("-------------");
     delay(300);     
     if(digitalRead(BTN_PIN2) == HIGH) {
      break;
     }
  }
}

void myDoubleClickHandlerUp() {
  for(MAX_BRIGHTNESS; MAX_BRIGHTNESS <= 220; MAX_BRIGHTNESS =  MAX_BRIGHTNESS + 10) {
    FastLED.setBrightness(MAX_BRIGHTNESS);
     Serial.println(MAX_BRIGHTNESS);
     Serial.println("-------------");
     delay(300);
     if(digitalRead(BTN_PIN) == HIGH) {
      break;
     }
  }
}
