/*
  ==============================================================================

    BasicWaveforms.cpp
    Created: 9 May 2021 8:57:43am
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/BasicWaveforms.h"
#include "util/Utils.h"
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

BasicWaveforms::BasicWaveforms() {
  setupSine();
  setupSaw();
  setupSquare();
}

void BasicWaveforms::setupSquare() {
  for (int i = 0; i < size; i++) {
    float value = (fmod(i, size)) >= size / 2 ? -1 : 1;
    square.array[i] = value;
  }
}

void BasicWaveforms::setupSaw() {
  for (int i = 0; i < size; i++) {
    float x = fmod(i, size) * 2.0f;
    saw.array[i] = (x - (size)) / (size);
  }
}

void BasicWaveforms::setupSine() {
  for (int i = 0; i < size; i++)
    sine.array[i] = sinf(static_cast<float>(M_PI * 2.0f * i / size));
}

float BasicWaveforms::getSine(float x) { return Utils::decimalSubscript(sine.array, x * size); }
float BasicWaveforms::getSaw(float x) { return Utils::decimalSubscript(saw.array, x * size); }
float BasicWaveforms::getSquare(float x) { return Utils::decimalSubscript(square.array, x); }
float BasicWaveforms::getTriangle(float x) { return Utils::decimalSubscript(triangle.array, x); }
