/*
  ==============================================================================

    WaveTableConstants.h
    Created: 18 Feb 2021 4:58:04pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "WaveTable.h"

class WaveTableConstants {
public:
  enum class WaveTableType {
    bandlimitedSawtooth,
    bandlimitedSquare,
    bandlimitedTriangle,

    sine,
    square,
    triangle,
    sawtooth,

    count
  };

  static WaveTable* getWaveTable(WaveTableType type);
  static void load(int sampleRate);
private:
  static const int lfoArrayLength = 100;

  static void setupSquare();
  static void setupTriangle();
  static void setupSawtooth();
  static void setupTestWave();

  static void setupSineWaveTable(int tableLength);
  static void fillSawtoothHarmonics(float* inputArray, int length, int harmonics);
  static void fillSineHarmonics(float* inputArray, int length);
  static void fillSquareHarmonics(float* inputArray, int length, int harmonics);
  static void fillTriangleHarmonics(float* inputArray, int length, int harmonics);
  static void fft(int length, float* realArray, float* imaginaryArray);

  static void normalizeWaveform(float* waveform, int length);

  static void loadBandlimitedWaveTables(int sampleRate);

  inline static WaveTable bandlimitedSawtooth;
  inline static WaveTable bandlimitedTriangle;
  inline static WaveTable bandlimitedSquare;

  inline static WaveTable sine;
  inline static WaveTable square;
  inline static WaveTable triangle;
  inline static WaveTable sawtooth;
  inline static WaveTable testWave;
};