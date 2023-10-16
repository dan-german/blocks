/*
  ==============================================================================

    OnePoleFilter.h
    Created: 17 Jan 2022 6:55:58pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class OnePoleFilter {
public:
  float alpha = 1.0f;
  float beta = 1.0f;
  float sampleRate = 44100.0f;
  float frequency;
  enum class Type { LPF1, HPF1 };
  Type type = Type::LPF1;
private:
  float z1 = 1.0f;
  float gamma = 1.0f;
  float delta = 0.0f;
  float epsilon = 0.0f;
  float da0 = 1.0f;
  float feedback = 0.0f;
public:
  void  setFeedback(float value) { this->feedback = value; }
  float getFeedback() { return beta * (z1 + feedback * delta); }
  void reset() { z1 = 0.0f; feedback = 0.0f; }

  inline void update() {
    float wd = 2 * float(M_PI) * frequency;
    float T = 1.0f / sampleRate;
    float wa = (2.0f / T) * tan(wd * T / 2.0f);
    float g = wa * T / 2.0f;

    alpha = g / (1.0f + g);
  }

  inline float process(float sample) {
    sample = sample * gamma + feedback + epsilon * getFeedback();
    float vn = (da0 * sample - z1) * alpha;
    float lpf = vn + z1;
    z1 = vn + lpf;

    if (type == Type::LPF1)
      return lpf;
    else
      return sample - lpf;
  }
};