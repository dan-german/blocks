/*
  ==============================================================================

    Processor.h
    Created: 4 Mar 2021 7:22:02am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "dsp/Parameter.h"
#include "dsp/Voice.h"

using Block = Model::Block;

class Processor: public Voice::Listener {
public:
  Array<std::shared_ptr<Parameter>> parameters;
  std::shared_ptr<Module> module;

  AudioSampleBuffer buffer;
  double bpm = 120;

  Processor(int bufferSize);
  virtual ~Processor() override;

  float getNextValue();
  float getValue(int index) { return buffer.getSample(0, index); }
  std::shared_ptr<Parameter> getParameter(int index) { return parameters[index]; }
  void process(int samples);
  void noteStarted(Voice* voice, float frequencyInHertz) override {}
  void noteStopped(bool allowTailOff) override {}
  void setModule(std::shared_ptr<Module> module);
  void process();

  virtual bool isActive() { return false; }
  virtual void setParameterValue(int index, float value) { getParameter(index)->setValue(value); }
  virtual void process(AudioSampleBuffer& inputBuffer);
  virtual void prepareToPlay(double sampleRate, int bufferSize);
  virtual void setPhase(int64 samples) { }
protected:
  virtual inline float _getNextValue() { return 0; }
  float sampleRate;
  int phase = 0;
};
