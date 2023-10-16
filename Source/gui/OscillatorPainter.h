/*
  ==============================================================================

    OscillatorPainter.h
    Created: 8 Nov 2020 1:39:46am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "settings/Constants.h"
#include "gui/GraphicsTimer.h"

class OscillatorPainter: public Component, GraphicsTimer {
public:
  enum class WaveformType { saw, sine, square, triangle, noise };
  WaveformType waveformType;

  OscillatorPainter();
  ~OscillatorPainter() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void setUnison(int unison);

  Colour waveColour;
  float thickness = 2.5f;

  void setWaveformType(WaveformType waveformType);
  float speed = 30.0f;
  float cycles = 3.0f;
private:
  float resolution = 0.5f;
  Path paths[4];
  int unison = 1;
  float offset = 0;

  void update(const float secondsSinceLastUpdate) override;

  inline float sine(float x) {
    float width = (float)getWidth();
    float pi = static_cast<const float>(std::acos(-1));
    float fullCircleInRadians = 2 * pi;

    float unit = fullCircleInRadians / width * cycles;
    return sin(x * unit);
  }

  inline float saw(float phase) {
    float width = (float)getWidth();

    float x = fmod(phase * cycles, width) * 2.0f;
    return (width - x) / (width);
  }

  inline float square(float x) {
    float width = (float)getWidth();

    float value = (fmod(x, width / cycles)) >= width / cycles / 2 ? -1 : 1;
    return value;
  }

  inline float triangle(float x) {
    float width = (float)getWidth();
    float phase = fmod(x * cycles, width);

    float value;
    if (phase >= width / 2) {
      value = phase / width;
    } else {
      value = 1.0f - phase / width;
    }

    return jmap(value, 0.5f, 1.0f, -1.0f, 1.0f);
  }

  inline float getWaveform(float x) {
    switch (waveformType) {
    case WaveformType::sine:
      return sine(x);
    case WaveformType::saw:
      return saw(x);
    case WaveformType::square:
      return square(x);
    case WaveformType::triangle:
      return triangle(x);
    case WaveformType::noise:
      return 1.0f - 2.0f * rand() / RAND_MAX;
    }
    return 0.f; 
  }

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscillatorPainter)
};
