/*
  ==============================================================================

    Modulation.h
    Created: 11 May 2021 7:43:33am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;

namespace Model {
class Module;
struct Modulation {
  Modulation(Module* target, Module* modulator, int parameterIndex, float magnitude, int number, bool bipolar = false);
  Modulation(int number);
  ~Modulation();

  std::shared_ptr<RangedAudioParameter> magnitudeParameter;
  std::shared_ptr<RangedAudioParameter> bipolarParameter;

  int id = 0;
  int number;
  String name;
  std::shared_ptr<Module> source;
  std::shared_ptr<Module> target;
  String parameterID;
  int parameterIndex;

  float getMagnitude() { return magnitudeParameter->getNormalisableRange().convertFrom0to1(magnitudeParameter->getValue()); }
  float getBipolar() { return bipolarParameter->getNormalisableRange().convertFrom0to1(bipolarParameter->getValue()); }
  bool isOscGainEnvelope();

  void reset();
  void setMagnitude(float magnitude);
  void setPolarity(bool bipolar);
  bool operator==(Modulation const& rhs) const;
};
}
