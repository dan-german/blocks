/*
  ==============================================================================

    InspectorComponent.h
    Created: 19 Nov 2020 12:24:13am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Module.h"
#include "InspectorSlider.h"
#include "module_new.h"
#include "vital/common/synth_parameters.h"

using namespace Model;

class InspectorComponent: public Component, Slider::Listener {
public:
  struct Listener;

  int calculateWidth();
  int sliderWidth = 80;
  Listener* delegate;
  Slider::Listener* listener;

  InspectorComponent();
  ~InspectorComponent() override;

  void resized() override;

  void setConfiguration(std::shared_ptr<model::Module> module);
  void setModulationIndicatorValue(int parameterIndex, int modulatorIndex, float value, float magnitude);
  void setModulationIndicatorPolarity(int parameterIndex, int modulatorIndex, bool bipolar);
  OwnedArray<InspectorSlider>& getSliders();
private:
  OwnedArray<InspectorSlider> parameterSliders;
  std::map<Slider*, std::string> slider_to_parameter_name_map_;

  void onDelayAdjusted(std::shared_ptr<model::Module> module, vital::ValueDetails parameter, double value); 
  void setSliderAsTempo(std::shared_ptr<model::Module> module, InspectorSlider* slider, std::string parameter_name) const;
  void setSliderAsFrequency(std::shared_ptr<model::Module> module, InspectorSlider* slider, std::string parameter_name) const;
  void spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module);
  void sliderValueChanged(Slider* slider) override;
  void updateSize();
  void resetInspector();
  int getIndexOfSlider(const Slider* slider) const;

  void sliderDragStarted(Slider* slider) override;
  void sliderDragEnded(Slider* slider) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InspectorComponent)
};

struct InspectorComponent::Listener {
  virtual void inspectorChangedParameter(int index, float value) = 0;
  virtual void inspectorGestureChanged(std::string parameter_name, bool started) = 0;
  virtual ~Listener() = default;
};
