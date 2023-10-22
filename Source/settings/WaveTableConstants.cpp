/*
  ==============================================================================

    WaveTableConstants.cpp
    Created: 18 Feb 2021 4:58:04pm
    Author:  Dan German

  ==============================================================================
*/

#include "settings/WaveTableConstants.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void WaveTableConstants::fillSineHarmonics(float* inputArray, int length) {
  for (int i = 0; i < length; i++)
    inputArray[i] = 0;

  inputArray[1] = 1;
  inputArray[1] = -1;
}

void WaveTableConstants::fillSawtoothHarmonics(float* inputArray, int length, int harmonics) {
  for (int i = 0; i < length; i++)
    inputArray[i] = 0;

  harmonics = std::min(length / 2, harmonics);

  for (int i = 1, j = length - 1; i <= harmonics; i++, j--) {
    float temp = -1.0 / i;
    inputArray[i] = -temp;
    inputArray[j] = temp;
  }
}

void WaveTableConstants::fillTriangleHarmonics(float* inputArray, int length, int harmonics) {
  for (int i = 0; i < length; i++)
    inputArray[i] = 0;

  float sign = 1;
  for (int idx = 1, jdx = length - 1; idx <= harmonics; idx++, jdx--) {
    float temp = idx & 0x01 ? 1.0 / (idx * idx) * (sign = -sign) : 0.0;
    inputArray[idx] = -temp;
    inputArray[jdx] = temp;
  }
}

void WaveTableConstants::fillSquareHarmonics(float* inputArray, int length, int harmonics) {
  for (int i = 0; i < length; i++)
    inputArray[i] = 0;

  for (int idx = 1, jdx = length - 1; idx <= harmonics; idx++, jdx--) {
    float temp = idx & 0x01 ? 1.0 / idx : 0.0;
    inputArray[idx] = -temp;
    inputArray[jdx] = temp;
  }
}

void WaveTableConstants::load(int sampleRate) {
  loadBandlimitedWaveTables(sampleRate);

  setupSquare();
  setupTriangle();
  setupSawtooth();
  setupTestWave();
}

void WaveTableConstants::loadBandlimitedWaveTables(int sampleRate) {
  // calc number of harmonics where the highest harmonic baseFreq and lowest alias an octave higher would meet
  float baseFrequency = 5;
  int harmonics = static_cast<int>(44100 / (3.0f * baseFrequency) + 0.5f);

  int nearestPowerOfTwo = static_cast<int>(pow(2, ceil(log(harmonics) / log(2))));
  int tableLength = nearestPowerOfTwo * 4; // Double for the sample rate & oversampling

  std::vector<float> sawHarmonics(tableLength);
  std::vector<float> sawWaveform(tableLength);

  std::vector<float> squareHarmonics(tableLength);
  std::vector<float> squareWaveform(tableLength);

  std::vector<float> triangleHarmonics(tableLength);
  std::vector<float> triangleWaveform(tableLength);

  float topFrequency = baseFrequency * 2.0f / sampleRate;

  setupSineWaveTable(tableLength);

  while (harmonics >= 1) {
    for (int i = 0; i < tableLength; i++) {
      sawWaveform[i] = 0;
      squareWaveform[i] = 0;
      triangleWaveform[i] = 0;
    }
    // Make an array of various harmonics
    fillSawtoothHarmonics(sawHarmonics.data(), tableLength, harmonics);
    fillSquareHarmonics(squareHarmonics.data(), tableLength, harmonics);
    fillTriangleHarmonics(triangleHarmonics.data(), tableLength, harmonics);
    // Generate waveforms out of the harmonics using iFFT
    fft(tableLength, sawHarmonics.data(), sawWaveform.data());
    fft(tableLength, squareHarmonics.data(), squareWaveform.data());
    fft(tableLength, triangleHarmonics.data(), triangleWaveform.data());
    // Normalized waveforms to +-1 range
    normalizeWaveform(sawWaveform.data(), tableLength);
    normalizeWaveform(squareWaveform.data(), tableLength);
    normalizeWaveform(triangleWaveform.data(), tableLength);
    // Add waveforms to corresponding wave tables
    bandlimitedSawtooth.addWaveform(tableLength, sawWaveform.data(), topFrequency);
    bandlimitedSquare.addWaveform(tableLength, squareWaveform.data(), topFrequency);
    bandlimitedTriangle.addWaveform(tableLength, triangleWaveform.data(), topFrequency);

    topFrequency *= 2;
    harmonics /= 2;
  }
}

// Makes waveform values range between +-1
void WaveTableConstants::normalizeWaveform(float* waveform, int length) {
  // Find max absolute value in current waveform
  float max = 0;
  for (int i = 0; i < length; i++) {
    float absoluteValue = fabs(waveform[i]);
    if (absoluteValue > max)
      max = absoluteValue;
  }
  // Apply new scaling
  float scale = 1.0f / max * .999f;
  for (int i = 0; i < length; i++)
    waveform[i] = waveform[i] * scale;
}

// the fft algorithm used in earlevel's wave table tutorial - https://www.earlevel.com/
void WaveTableConstants::fft(int length, float* realArray, float* imaginaryArray) {
  int i, j, k, L;            /* ids */
  int M, TEMP, LE, LE1, ip;  /* M = log length */
  int NV2, NM1;
  float t;               /* temp */
  float Ur, Ui, Wr, Wi, Tr, Ti;
  float Ur_old;

  // if ((length > 1) && !(length & (length - 1)))   // make sure we have a power of 2

  NV2 = length >> 1;
  NM1 = length - 1;
  TEMP = length; /* get M = log length */
  M = 0;
  while (TEMP >>= 1) ++M;

  /* shuffle */
  j = 1;
  for (i = 1; i <= NM1; i++) {
    if (i < j) {             /* swap a[i] and a[j] */
      t = realArray[j - 1];
      realArray[j - 1] = realArray[i - 1];
      realArray[i - 1] = t;
      t = imaginaryArray[j - 1];
      imaginaryArray[j - 1] = imaginaryArray[i - 1];
      imaginaryArray[i - 1] = t;
    }

    k = NV2;             /* bit-reversed counter */
    while (k < j) {
      j -= k;
      k /= 2;
    }

    j += k;
  }

  LE = 1.;
  for (L = 1; L <= M; L++) {            // stage L
    LE1 = LE;                         // (LE1 = LE/2)
    LE *= 2;                          // (LE = 2^L)
    Ur = 1.0;
    Ui = 0.;
    Wr = cos(M_PI / (float)LE1);
    Wi = -sin(M_PI / (float)LE1); // Cooley, Lewis, and Welch have "+" here
    for (j = 1; j <= LE1; j++) {
      for (i = j; i <= length; i += LE) { // butterfly
        ip = i + LE1;
        Tr = realArray[ip - 1] * Ur - imaginaryArray[ip - 1] * Ui;
        Ti = realArray[ip - 1] * Ui + imaginaryArray[ip - 1] * Ur;
        realArray[ip - 1] = realArray[i - 1] - Tr;
        imaginaryArray[ip - 1] = imaginaryArray[i - 1] - Ti;
        realArray[i - 1] = realArray[i - 1] + Tr;
        imaginaryArray[i - 1] = imaginaryArray[i - 1] + Ti;
      }
      Ur_old = Ur;
      Ur = Ur_old * Wr - Ui * Wi;
      Ui = Ur_old * Wi + Ui * Wr;
    }
  }
}

// Sine has 1 harmonic so he gets special treatment
void WaveTableConstants::setupSineWaveTable(int tableLength) {
    std::vector<float> sineHarmonics(tableLength);
    std::vector<float> sineWaveform(tableLength, 0.0);

  fillSineHarmonics(sineHarmonics.data(), tableLength);
  fft(tableLength, sineHarmonics.data(), sineWaveform.data());
  normalizeWaveform(sineWaveform.data(), tableLength);
  sine.addWaveform(tableLength, sineWaveform.data(), 1);
}

void WaveTableConstants::setupSquare() {
  std::vector<float> sqr(lfoArrayLength);


  for (int i = 0; i < lfoArrayLength; i++)
    sqr[i] = i < lfoArrayLength / 2.0f ? 1.0f : -1.0f;

  square.addWaveform(lfoArrayLength, sqr.data(), 1);
}

void WaveTableConstants::setupSawtooth() {
  std::vector<float> saw(lfoArrayLength);

  for (int i = 0; i < lfoArrayLength; i++)
    saw[i] = i / ((float)lfoArrayLength - 1.0f) * 2.0f - 1.0f;

  sawtooth.addWaveform(lfoArrayLength, saw.data(), 1);
}

void WaveTableConstants::setupTriangle() {
  std::vector<float> tri(lfoArrayLength);

  for (int i = 0; i < lfoArrayLength; i++)
    tri[i] = 2 * abs(i / ((float)lfoArrayLength - 1.0f) * 2.0f - 1.0f) - 1.0f;

  triangle.addWaveform(lfoArrayLength, tri.data(), 1);
}

void WaveTableConstants::setupTestWave() {
  int waveLength = 3;
  std::vector<float> test(waveLength);

  test[0] = 0.1f;
  test[1] = 0.2f;
  test[2] = 0.3f;

  testWave.addWaveform(waveLength, test.data(), 1);
}

WaveTable* WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType type) {
  switch (type) {
  case WaveTableType::bandlimitedSawtooth:
    return &bandlimitedSawtooth;
  case WaveTableType::bandlimitedSquare:
    return &bandlimitedSquare;
  case WaveTableType::bandlimitedTriangle:
    return &bandlimitedTriangle;
  case WaveTableType::sine:
    return &sine;
  case WaveTableType::square:
    return &square;
  case WaveTableType::triangle:
    return &triangle;
  case WaveTableType::sawtooth:
    return &sawtooth;
  case WaveTableType::count:
    return nullptr;
  }
  return nullptr; 
}
