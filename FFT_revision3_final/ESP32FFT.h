#ifndef ESP32FFT_h 
#define ESP32FFT_h

#include "Arduino.h"
#include <stdlib.h>
#include <stdio.h>
#include "FastLED.h"

#define TRASH_HOLD 12000
#define NUM_LEDS 300

CRGB leds[NUM_LEDS];

#define BTN_PIN2   17            // For change move detection and microphone
OneButton btn2 = OneButton(BTN_PIN2, true, true);
bool isRunning2 = false;
bool isRunning = false;
uint8_t patternCounter2 = 0;

void nextPattern2() {
  isRunning = false;
  //isRunning2 = true;
  patternCounter2 = (patternCounter2 + 1) % 2;
  //Serial.println("calling nextPattern2()");
  
}

enum class FFTDirection
{
  Reverse,
  Forward
};

enum class FFTWindow 
{          
  
  Rectangle,     
  Hamming,      
  Triangle,    
  Hann,      
  Nuttall,      
  Blackman,     
  Blackman_Nuttall, 
  Blackman_Harris,  
  Flat_top,     
  Welch       
};

template <typename T>
class EspFFT
{
public:
  // Constructor
  EspFFT(T *vReal, T *vImag, uint_fast16_t samples, T samplingFrequency, T *windowWeighingFactors = nullptr)
    : _vReal(vReal)
    , _vImag(vImag)
    , _samples(samples)
    , _samplingFrequency(samplingFrequency)
    , _windowWeighingFactors(windowWeighingFactors)
  {
    // Calculates the base 2 logarithm of sample count
    _power = 0;
    while (((samples >> _power) & 1) != 1)
    {
      _power++;
    }
  }

  // Destructor
  ~EspFFT()
  {
  }

  // Computes in-place complex-to-complex FFT
  void compute(FFTDirection dir) const
  {
    // Reverse bits /
    uint_fast16_t j = 0;
    for (uint_fast16_t i = 0; i < (this->_samples - 1); i++)
    {
      if (i < j)
      {
        Swap(this->_vReal[i], this->_vReal[j]);
        if (dir == FFTDirection::Reverse)
        {
          Swap(this->_vImag[i], this->_vImag[j]);  
        }
      }
      uint_fast16_t k = (this->_samples >> 1);
      while (k <= j)
      {
        j -= k;
        k >>= 1;
      }
      j += k;
    }
    
    // Compute the FFT
    T c1 = -1.0;
    T c2 = 0.0;
    uint_fast16_t l2 = 1;
    for (uint_fast8_t l = 0; (l < this->_power); l++)
    {
      uint_fast16_t l1 = l2;
      l2 <<= 1;
      T u1 = 1.0;
      T u2 = 0.0;
      for (j = 0; j < l1; j++)
      {
        for (uint_fast16_t i = j; i < this->_samples; i += l2)
        {
          uint_fast16_t i1 = i + l1;
          T t1 = u1 * this->_vReal[i1] - u2 * this->_vImag[i1];
          T t2 = u1 * this->_vImag[i1] + u2 * this->_vReal[i1]; // This code calculates the FFT. This is done by consistently applying the "divide and rule" algorithm.
          this->_vReal[i1] = this->_vReal[i] - t1;              //  A divide-and-conquer algorithm divides the input signal into two halves, calculates the FFT of each half, and then recombines the results.
          this->_vImag[i1] = this->_vImag[i] - t2;
          this->_vReal[i] += t1;
          this->_vImag[i] += t2;
        }
        T z = ((u1 * c1) - (u2 * c2));
        u2 = ((u1 * c2) + (u2 * c1));
        u1 = z;
      }
      T cTemp = 0.5 * c1;
      c2 = sqrt(0.5 - cTemp);
      c1 = sqrt(0.5 + cTemp);
      c2 = dir == FFTDirection::Forward ? -c2 : c2;
    }
    
    // Scaling for reverse transform
    if (dir != FFTDirection::Forward)
    {
      for (uint_fast16_t i = 0; i < this->_samples; i++)
      {
        this->_vReal[i] /= this->_samples;
        this->_vImag[i] /= this->_samples;
      }
    }
  }

  void complexToMagnitude(int frequency, int N) const 
  {
    int highest_frequency = 1;
    int highest_amplitude;
    // vM is half the size of vReal and vImag
    for (uint_fast16_t i = 1; i < this->_samples / 2; i++)
    {
      this->_vReal[i] = sqrt(sq(this->_vReal[i]) + sq(this->_vImag[i]))/10000;
      if (this->_vReal[i] > this->_vReal[highest_frequency]) {
      highest_frequency = i;
    }
      Serial.println(i * frequency / N);
      Serial.println(this->_vReal[i]);
    }
    Serial.println("highest_frequency: ");
    Serial.println(highest_frequency * frequency / N);
    Serial.println(this->_vReal[highest_frequency]);
//    Serial.println("ENDING");


    highest_amplitude = this->_vReal[highest_frequency];

      for(int i = 1; i < 128; i++) {
    if(this->_vReal[i] < TRASH_HOLD) {
      this->_vReal[i] = 0;
      //Serial.println("ENDING");
    }
    else {
      this->_vReal[i] = 255;
    }
    //this->_vReal[i] = map(this->_vReal[i], 0, highest_amplitude, 0, 255);
  }

  float hue1 = 126;
  float hue2 = 233;

  if (digitalRead(BTN_PIN2) == LOW) { 
      Serial.println("final 1");
      isRunning2 = false;
      nextPattern2();
    }

  
  for(int i = 0; i < 20; i+=2) {
    leds[i] = CHSV(224, 255, this->_vReal[i/2]);  
    leds[i+1] = CHSV(224, 255, this->_vReal[i/2]); 
  }

  for(int i = 30; i < 169; i+=3) {
    leds[i-10] = CHSV(hue1, 255, this->_vReal[i/3]);  
    leds[i-10+1] = CHSV(hue1, 255, this->_vReal[i/3]); 
    leds[i-10+2] = CHSV(hue1, 255, this->_vReal[i/3]); 
    hue1-=2;
  }

  for(int i = 114; i < 253; i+=2) {
    leds[i+46] = CHSV(hue2, 255, this->_vReal[i/2]);  
    leds[i+46+1] = CHSV(hue2, 255, this->_vReal[i/2]); 
    hue2-=1.5;
  }
  
  btn2.tick();
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 1);
  }

  void dcRemoval() const 
  {
    // calculate the mean of vData
    T mean = 0;
    for (uint_fast16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
    {
      mean += this->_vReal[i];
    }
    mean /= this->_samples;
    // Subtract the mean from vData
    for (uint_fast16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
    {
      this->_vReal[i] -= mean;
    }
  }

  void windowing(FFTWindow windowType, FFTDirection dir, bool withCompensation = false) 
  {                                                                                    
    // check if values are already pre-computed for the correct window type and compensation
    if (_windowWeighingFactors && _weighingFactorsComputed &&
      _weighingFactorsFFTWindow == windowType &&
      _weighingFactorsWithCompensation == withCompensation)
    {
      // yes. values are precomputed
      if (dir == FFTDirection::Forward)
      {
        for (uint_fast16_t i = 0; i < (this->_samples >> 1); i++)
        {
          this->_vReal[i] *= _windowWeighingFactors[i];
          this->_vReal[this->_samples - (i + 1)] *= _windowWeighingFactors[i];
        }
      }
      else
      {
        for (uint_fast16_t i = 0; i < (this->_samples >> 1); i++)
        {
          this->_vReal[i] /= _windowWeighingFactors[i];
          this->_vReal[this->_samples - (i + 1)] /= _windowWeighingFactors[i];
        }
      }
    }
    else
    {
      // no. values need to be pre-computed or applied
      T samplesMinusOne = (T(this->_samples) - 1.0);
      T compensationFactor = _WindowCompensationFactors[static_cast<uint_fast8_t>(windowType)];
      for (uint_fast16_t i = 0; i < (this->_samples >> 1); i++)
      {
        T indexMinusOne = T(i);
        T ratio = (indexMinusOne / samplesMinusOne);
        T weighingFactor = 1.0;
        // Compute and record weighting factor
        switch (windowType)
        {
        case FFTWindow::Rectangle: // rectangle (box car)
          weighingFactor = 1.0;
          break;
        case FFTWindow::Hamming: // hamming
          weighingFactor = 0.54 - (0.46 * cos(TWO_PI * ratio));
          break;
        case FFTWindow::Hann: // hann
          weighingFactor = 0.54 * (1.0 - cos(TWO_PI * ratio));
          break;
        case FFTWindow::Triangle: // triangle (Bartlett)
          weighingFactor = 1.0 - ((2.0 * abs(indexMinusOne - (samplesMinusOne / 2.0))) / samplesMinusOne);
          break;
        case FFTWindow::Nuttall: // nuttall
          weighingFactor = 0.355768 - (0.487396 * (cos(TWO_PI * ratio))) + (0.144232 * (cos(FOUR_PI * ratio))) - (0.012604 * (cos(SIX_PI * ratio)));
          break;
        case FFTWindow::Blackman: // blackman
          weighingFactor = 0.42323 - (0.49755 * (cos(TWO_PI * ratio))) + (0.07922 * (cos(FOUR_PI * ratio)));
          break;
        case FFTWindow::Blackman_Nuttall: // blackman nuttall
          weighingFactor = 0.3635819 - (0.4891775 * (cos(TWO_PI * ratio))) + (0.1365995 * (cos(FOUR_PI * ratio))) - (0.0106411 * (cos(SIX_PI * ratio)));
          break;
        case FFTWindow::Blackman_Harris: // blackman harris
          weighingFactor = 0.35875 - (0.48829 * (cos(TWO_PI * ratio))) + (0.14128 * (cos(FOUR_PI * ratio))) - (0.01168 * (cos(SIX_PI * ratio)));
          break;
        case FFTWindow::Flat_top: // flat top
          weighingFactor = 0.2810639 - (0.5208972 * cos(TWO_PI * ratio)) + (0.1980399 * cos(FOUR_PI * ratio));
          break;
        case FFTWindow::Welch: // welch
          weighingFactor = 1.0 - sq((indexMinusOne - samplesMinusOne / 2.0) / (samplesMinusOne / 2.0));
          break;
        }
        if (withCompensation)
        {
          weighingFactor *= compensationFactor;
        }
        if (_windowWeighingFactors)
        {
          _windowWeighingFactors[i] = weighingFactor;
        }
        if (dir == FFTDirection::Forward)
        {
          this->_vReal[i] *= weighingFactor;
          this->_vReal[this->_samples - (i + 1)] *= weighingFactor;
        }
        else
        {
          this->_vReal[i] /= weighingFactor;
          this->_vReal[this->_samples - (i + 1)] /= weighingFactor;
        }
      }
      // mark cached values as pre-computed
      _weighingFactorsFFTWindow = windowType;
      _weighingFactorsWithCompensation = withCompensation;
      _weighingFactorsComputed = true;
    }
  }

  T majorPeak() const   //  the code finds the main peak frequency of the signal. The main peak frequency is the frequency with the most power in the signal.
  {
    T maxY = 0;
    uint_fast16_t IndexOfMaxY = 0;
    //If sampling_frequency = 2 * max_frequency in signal,
    //value would be stored at position samples/2
    for (uint_fast16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
    {
      if ((this->_vReal[i - 1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i + 1]))
      {
        if (this->_vReal[i] > maxY)
        {
          maxY = this->_vReal[i];
          IndexOfMaxY = i;
        }
      }
    }
    T delta = 0.5 * ((this->_vReal[IndexOfMaxY - 1] - this->_vReal[IndexOfMaxY + 1]) / (this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]));
    T interpolatedX = ((IndexOfMaxY + delta) * this->_samplingFrequency) / (this->_samples - 1);
    if (IndexOfMaxY == (this->_samples >> 1))
    {
      //To improve calculation on edge values
      interpolatedX = ((IndexOfMaxY + delta) * this->_samplingFrequency) / (this->_samples);
    }
    // returned value: interpolated frequency peak apex
    return interpolatedX;
  }

  void majorPeak(T &frequency, T &value) const
  {
    T maxY = 0;
    uint_fast16_t IndexOfMaxY = 0;
    //If sampling_frequency = 2 * max_frequency in signal,
    //value would be stored at position samples/2
    for (uint_fast16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
    {
      if ((this->_vReal[i - 1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i + 1]))
      {
        if (this->_vReal[i] > maxY)
        {
          maxY = this->_vReal[i];
          IndexOfMaxY = i;
        }
      }
    }
    T delta = 0.5 * ((this->_vReal[IndexOfMaxY - 1] - this->_vReal[IndexOfMaxY + 1]) / (this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]));
    T interpolatedX = ((IndexOfMaxY + delta) * this->_samplingFrequency) / (this->_samples - 1);
    if (IndexOfMaxY == (this->_samples >> 1))
    {
      //To improve calculation on edge values
      interpolatedX = ((IndexOfMaxY + delta) * this->_samplingFrequency) / (this->_samples);
    }
    // returned value: interpolated frequency peak apex
    frequency = interpolatedX;
    value = abs(this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]);
  }

private:
  static const T _WindowCompensationFactors[10];

  // Mathematial constants
#ifndef TWO_PI
  static constexpr T TWO_PI = 6.28318531; // might already be defined in Arduino.h
#endif
  static constexpr T FOUR_PI = 12.56637061;
  static constexpr T SIX_PI = 18.84955593;

  static inline void Swap(T &x, T &y)
  {
    T temp = x;
    x = y;
    y = temp;
  }

  /* Variables */
  T *_vReal = nullptr;
  T *_vImag = nullptr;
  uint_fast16_t _samples = 0;
  T _samplingFrequency = 0;
  T *_windowWeighingFactors = nullptr;
  FFTWindow _weighingFactorsFFTWindow;
  bool _weighingFactorsWithCompensation = false;
  bool _weighingFactorsComputed = false;
  uint_fast8_t _power = 0;
};


template <typename T>
const T EspFFT<T>::_WindowCompensationFactors[10] = {
  1.0000000000 * 2.0, // rectangle (Box car)
  1.8549343278 * 2.0, // hamming
  1.8554726898 * 2.0, // hann
  2.0039186079 * 2.0, // triangle (Bartlett)
  2.8163172034 * 2.0, // nuttall
  2.3673474360 * 2.0, // blackman
  2.7557840395 * 2.0, // blackman nuttall
  2.7929062517 * 2.0, // blackman harris
  3.5659039231 * 2.0, // flat top
  1.5029392863 * 2.0  // welch
};
#endif
