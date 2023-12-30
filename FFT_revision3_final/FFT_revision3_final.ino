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

// Push button connected between pin 16 and GND (no resistor required)
OneButton btn = OneButton(BTN_PIN, true, true);

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
#define LED_PIN 12
int MAX_BRIGHTNESS = 100; 
#define FRAME_RATE 30

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

  audioInfo.autoLevel(AudioAnalysis::EXPONENTIAL_FALLOFF, 1, 255, 5000); 
  audioInfo.bandPeakFalloff(AudioAnalysis::EXPONENTIAL_FALLOFF, 1);    
  audioInfo.vuPeakFalloff(AudioAnalysis::ACCELERATE_FALLOFF, 1);        


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
