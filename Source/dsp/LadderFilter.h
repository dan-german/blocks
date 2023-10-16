/*
  ==============================================================================

    LadderFilter.h
    Created: 17 Jan 2022 7:33:36pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "dsp/OnePoleFilter.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class LadderFilter {
public:
  float frequency;
  float sampleRate;
  static inline float minK = 0.0f;
  static inline float maxK = 12.0f;
  float k = 2.5f; // 0 -> 4
  float alpha = 1.0f;

  OnePoleFilter lpf1;
  OnePoleFilter lpf2;
  OnePoleFilter lpf3;
  OnePoleFilter lpf4;

  float gamma;
  float a = 0.0f;
  float b = 0.0f;
  float c = 0.0f;
  float d = 0.0f;
  float e = 0.0f;

  enum class Type { LPF2, LPF4, HPF2, HPF4, BPF2, BPF4 };
  Type type = Type::LPF4;

  LadderFilter() {
    lpf1.type = OnePoleFilter::Type::LPF1;
    lpf2.type = OnePoleFilter::Type::LPF1;
    lpf3.type = OnePoleFilter::Type::LPF1;
    lpf4.type = OnePoleFilter::Type::LPF1;

    type = Type::LPF4;

    reset();
  }

  void setFrequency(float value) {
    this->frequency = value;
    lpf1.frequency = value;
    lpf2.frequency = value;
    lpf3.frequency = value;
    lpf4.frequency = value;
  }

  void reset() {
    lpf1.reset();
    lpf2.reset();
    lpf3.reset();
    lpf4.reset();
  }

  void update() {
    float wd = 2.0f * float(M_PI) * frequency;
    float T = 1.0f / sampleRate;
    float wa = (2.0f / T) * tan(wd * T / 2.0f);
    float g = wa * T / 2.0f;

    float G = g / (1.0f + g);

    lpf1.alpha = G;
    lpf2.alpha = G;
    lpf3.alpha = G;
    lpf4.alpha = G;

    lpf1.beta = G * G * G / (1.0f + g);
    lpf2.beta = G * G / (1.0f + g);
    lpf3.beta = G / (1.0f + g);
    lpf4.beta = 1.0f / (1.0f + g);

    gamma = G * G * G * G;

    alpha = 1.0f / (1.0f + k * gamma);

    if (type == Type::LPF4) {
      a = 0.0f; b = 0.0f; c = 0.0f; d = 0.0f; e = 1.0f;
    } else if (type == Type::LPF2) {
      a = 0.0f; b = 0.0f; c = 1.0f; d = 0.0f; e = 0.0f;
    } else if (type == Type::BPF4) {
      a = 0.0f; b = 0.0f; c = 4.0f; d = -8.0f; e = 4.0f;
    } else if (type == Type::BPF2) {
      a = 0.0f; b = 2.0f; c = -2.0f; d = 0.0f; e = 0.0f;
    } else if (type == Type::HPF4) {
      a = 1.0f; b = -4.0f; c = 6.0f; d = -4.0f; e = 1.0f;
    } else if (type == Type::HPF2) {
      a = 1.0f; b = -2.0f; c = 1.0f; d = 0.0f; e = 0.0f;
    }
  }

  float process(float sample) {
    float sigma = lpf1.getFeedback() + lpf2.getFeedback() + lpf3.getFeedback() + lpf4.getFeedback();

    float u = (sample - k * sigma) * alpha;

    float state1 = lpf1.process(u);
    float state2 = lpf2.process(state1);
    float state3 = lpf4.process(state2);
    float state4 = lpf1.process(state3);

    return a * u + b * state1 + c * state2 + d * state3 + e * state4;
  }
};