/*
  ==============================================================================

    InspectorComponent.cpp
    Created: 19 Nov 2020 12:24:13am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/InspectorComponent.h"
// #include "model/ModuleParameter.h"
#include "module_new.h"
#include "ui_utils.h"
#include "vital/synth_strings.h"

InspectorComponent::InspectorComponent() {}
InspectorComponent::~InspectorComponent() { }
void InspectorComponent::resized() { updateSize(); }
int InspectorComponent::calculateWidth() { return sliderWidth * (parameterSliders.size()); }
void InspectorComponent::sliderDragStarted(Slider* slider) { delegate->inspectorGestureChanged(slider_to_parameter_name_map_[slider], true); }
void InspectorComponent::sliderDragEnded(Slider* slider) { delegate->inspectorGestureChanged(slider_to_parameter_name_map_[slider], false); }

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
    if (module->id.type == "delay") {
      onDelayAdjusted(module, parameter, value);
    } else if (module->id.type == "phaser" || module->id.type == "chorus" || module->id.type == "flanger") {
      bool is_changing_sync = parameter.name == "sync";
      if (is_changing_sync) {
        auto frequency_slider = getSliders()[3];
        bool is_seconds = int(value) == 0;
        if (is_seconds) {
          setSliderAsFrequency(module, frequency_slider, "frequency");
        } else {
          setSliderAsTempo(module, frequency_slider, "tempo");
        }
      }
    }

    return UIUtils::getSliderTextFromValue(value, parameter);
  };

  slider->titleLabel.setText(parameter.display_name, dontSendNotification);
  parameterSliders.add(slider);
  slider_to_parameter_name_map_[&slider->slider] = parameter.name;
  addAndMakeVisible(slider);
  auto value = parameter.value_processor->value();
  slider->slider.setValue(value, dontSendNotification);
}

void InspectorComponent::onDelayAdjusted(std::shared_ptr<model::Module> module, vital::ValueDetails parameter, double value) {
  bool is_changing_sync_1 = parameter.name == "sync";
  bool is_changing_sync_2 = parameter.name == "sync 2";
  if (is_changing_sync_1 || is_changing_sync_2) {
    int tempo_slider = is_changing_sync_1 ? 4 : 6;
    std::string tempo_name = is_changing_sync_1 ? "tempo" : "tempo 2";
    std::string frequency_name = is_changing_sync_1 ? "frequency" : "frequency 2";

    auto frequency_slider = getSliders()[tempo_slider];
    bool is_seconds = int(value) == 0;
    if (is_seconds) {
      setSliderAsFrequency(module, frequency_slider, frequency_name);
    } else {
      setSliderAsTempo(module, frequency_slider, tempo_name);
    }
  }
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

void InspectorComponent::setSliderAsFrequency(std::shared_ptr<model::Module> module, InspectorSlider* slider, std::string parameter_name = "frequency") const {
  slider->titleLabel.setText("seconds", dontSendNotification);
  auto parameter = module->parameter_map_[parameter_name];
  slider->slider.textFromValueFunction = [slider, module, parameter](double value) {
    return UIUtils::getSliderTextFromValue(value, *parameter);
  };
  slider->slider.setRange(parameter->min, parameter->max);
  slider->slider.setValue(parameter->value_processor->value(), dontSendNotification);
}

void InspectorComponent::setSliderAsTempo(std::shared_ptr<model::Module> module, InspectorSlider* slider, std::string paramter_name = "tempo") const {
  slider->titleLabel.setText("tempo", dontSendNotification);
  slider->slider.textFromValueFunction = [slider, module](double value) {
    return juce::String(strings::kSyncedFrequencyNames[int(value)]);
  };
  auto parameter = module->parameter_map_[paramter_name];
  slider->slider.setRange(parameter->min, parameter->max, 1.0);
  slider->slider.setValue(parameter->value_processor->value(), dontSendNotification);
}