/*
  ==============================================================================

    InspectorComponent.cpp
    Created: 19 Nov 2020 12:24:13am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/InspectorComponent.h"
#include "model/ModuleParameter.h"
#include "module_new.h"
#include "ui_utils.h"
#include "vital/synth_strings.h"

InspectorComponent::InspectorComponent() {}
InspectorComponent::~InspectorComponent() { }
void InspectorComponent::resized() { updateSize(); }
int InspectorComponent::calculateWidth() { return sliderWidth * (parameterSliders.size()); }
void InspectorComponent::sliderDragStarted(Slider* slider) { delegate->inspectorGestureChanged(getIndexOfSlider(slider), true); }
void InspectorComponent::sliderDragEnded(Slider* slider) { delegate->inspectorGestureChanged(getIndexOfSlider(slider), false); }

void InspectorComponent::setConfiguration(std::shared_ptr<model::Module> module) {
  resetInspector();
  for (auto parameter : module->parameters_) spawnSlider(*parameter, module);
  updateSize();
}

void InspectorComponent::spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module) {
  if (parameter.hidden) return;
  auto slider = new InspectorSlider();
  double interval = 0.0;

  if (parameter.value_scale == ValueScale::kIndexed) {
    interval = 1.0;
  }

  slider->slider.setRange(parameter.min, parameter.max, interval);
  slider->slider.addListener(this);
  slider->slider.setNumDecimalPlacesToDisplay(parameter.decimal_places);

  slider->slider.textFromValueFunction = [this, parameter, module, slider](double value) {
    bool is_changing_sync = parameter.name == "sync";
    if (is_changing_sync) {
      auto frequency_slider = getSliders()[4];
      bool is_seconds = int(value) == 0;
      if (is_seconds) {
        setSliderAsFrequency(module, frequency_slider);
      } else {
        setSliderAsTempo(module, frequency_slider);
      }
    }
    return UIUtils::getSliderTextFromValue(value, parameter);
  };

  slider->titleLabel.setText(parameter.display_name, dontSendNotification);
  parameterSliders.add(slider);
  addAndMakeVisible(slider);
  slider->slider.setValue(parameter.val->value(), dontSendNotification);
}

void InspectorComponent::resetInspector() {
  for (auto slider : parameterSliders) removeChildComponent(slider);
  parameterSliders.clear(true);
}

void InspectorComponent::updateSize() {
  setBounds(getX(), getY(), calculateWidth(), getHeight());

  for (int i = 0; i < parameterSliders.size(); i++)
    parameterSliders[i]->setBounds(i * sliderWidth, 0, sliderWidth, getHeight());
}

void InspectorComponent::sliderValueChanged(Slider* slider) {
  auto value = slider->getValue();
  delegate->inspectorChangedParameter(getIndexOfSlider(slider), static_cast<float>(slider->getValue()));
}

inline int InspectorComponent::getIndexOfSlider(const Slider* slider) const {
  for (int i = 0; i < parameterSliders.size(); i++)
    if (&parameterSliders[i]->slider == slider)
      return i;

  return -1;
}

OwnedArray<InspectorSlider>& InspectorComponent::getSliders() { return parameterSliders; }

void InspectorComponent::setModulationIndicatorValue(int parameterIndex, int modulatorIndex, float value, float magnitude) {
  getSliders()[parameterIndex]->setModulatorValue(modulatorIndex, magnitude, value);
}

void InspectorComponent::setModulationIndicatorPolarity(int parameterIndex, int modulatorIndex, bool bipolar) {
  getSliders()[parameterIndex]->setModulatorBipolar(modulatorIndex, bipolar);
}

void InspectorComponent::setSliderAsFrequency(std::shared_ptr<model::Module> module, InspectorSlider* slider) const {
  slider->titleLabel.setText("seconds", dontSendNotification);
  slider->slider.textFromValueFunction = [slider, module](double value) {
    return UIUtils::getSliderTextFromValue(value, *(module->parameter_map_["frequency"]));
  };
  slider->slider.setRange(-2.0, 9.0);
}

void InspectorComponent::setSliderAsTempo(std::shared_ptr<model::Module> module, InspectorSlider* slider) const {
  slider->titleLabel.setText("tempo", dontSendNotification);
  slider->slider.textFromValueFunction = [slider, module](double value) {
    return juce::String(strings::kSyncedFrequencyNames[int(value)]);
  };
  slider->slider.setRange(0.0, 12.0, 1.0);
}