/*
  ==============================================================================

    NoiseGenerator.h
    Created: 13 Jul 2021 7:33:33pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <cstdlib>

struct NoiseGenerator {
  int sampleRate = 44100;
  float accumulator = 0;
  float frequency = 10.0f;
  float value = 0.0f;

  inline float getNextValue() {
    accumulator += frequency;

    if (accumulator >= sampleRate) {
      value = 1.0f - 2.0f * rand() / RAND_MAX;
      accumulator = 0;
    }

    return value;
  }
};
