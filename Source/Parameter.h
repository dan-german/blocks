/*
  ==============================================================================

    Parameter.h
    Created: 19 Apr 2021 12:16:39am
    Author:  Dan German

  ==============================================================================
*/

#include "ModulationInput.h"

using namespace std;

class Parameter { // TODO - rename to ProcessorParameter
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameter)
    float value = 1.0f;
public:
  RangedAudioParameter* juceParameter;
  NormalisableRange<float> range;
  Array<shared_ptr<ModulationInput>> modulationInputs;

  Parameter() { }
  Parameter(float defaultValue, float min, float max): value(defaultValue), range(min, max) { }
  ~Parameter() {  }

  inline void setValue(float newValue) { value = range.convertTo0to1(newValue); }
  inline void setRange(float min, float max) { range.start = min; range.end = max; }

  inline void connectModulation(shared_ptr<Processor> source, shared_ptr<Modulation> connection) {
    modulationInputs.add(make_shared<ModulationInput>(source, connection));
  }

  inline bool hasModulators() { return !modulationInputs.isEmpty(); }

  inline float getValue(int phase) {
    float finalValue = juceParameter->getValue();

    if (hasModulators())
      finalValue = std::clamp(juceParameter->getValue() + getModulationValue(phase), 0.0f, 1.0f);

    return juceParameter->getNormalisableRange().convertFrom0to1(finalValue);
  }

  inline bool isEnvelopeActive() {
    for (auto mi : modulationInputs) if (mi->isActive()) return true;
    return false;
  }

  inline float getModulationValue(int phase) {
    float modulationValue = 0.0f;

    for (auto modulator : modulationInputs)
      modulationValue += modulator->getValue(phase);

    return modulationValue;
  }

  void removeModulator(shared_ptr<Processor> modulator) {
    for (int i = 0; i < modulationInputs.size(); i++)
      if (modulationInputs[i]->source == modulator) modulationInputs.remove(i);
  }
};
