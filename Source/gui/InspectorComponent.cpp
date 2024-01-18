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

InspectorComponent::InspectorComponent() {}
InspectorComponent::~InspectorComponent() { }
void InspectorComponent::resized() { updateSize(); }
int InspectorComponent::calculateWidth() { return sliderWidth * (parameterSliders.size()); }
void InspectorComponent::sliderDragStarted(Slider* slider) { delegate->inspectorGestureChanged(getIndexOfSlider(slider), true); }
void InspectorComponent::sliderDragEnded(Slider* slider) { delegate->inspectorGestureChanged(getIndexOfSlider(slider), false); }

void InspectorComponent::setConfiguration(std::shared_ptr<model::Module> module) {
  resetInspector();
  for (auto parameter : module->parameters_) spawnSlider(*parameter);
  updateSize();
}

void InspectorComponent::spawnSlider(vital::ValueDetails parameter) {
  if (parameter.hidden) return;
  auto slider = new InspectorSlider();
  // auto audioParameter = parameter->audioParameter;

  // auto range = audioParameter->getNormalisableRange();
  double interval = 0.0;
  if (parameter.value_scale == ValueScale::kIndexed) {
    interval = 1.0;
  }

  slider->slider.setRange(parameter.min, parameter.max, interval);

  slider->slider.addListener(this);
  slider->titleLabel.setText(parameter.display_name, dontSendNotification);
  slider->slider.setNumDecimalPlacesToDisplay(parameter.decimal_places);

  // slider->box_slider_.slider.textFromValueFunction = [parameter](double value) { return getSliderTextFromValue(value, *parameter); };
  slider->slider.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, parameter); };

  // slider->slider.setSkewFactor(parameter->skew, false);
  // slider->slider.setTextValueSuffix(parameter->valueSuffix);

  // if (dynamic_cast<AudioParameterInt*>(audioParameter)) 
  //   slider->slider.setNumDecimalPlacesToDisplay(0);
  // else if (dynamic_cast<AudioParameterFloat*>(audioParameter))
  //   slider->slider.setNumDecimalPlacesToDisplay(2);

  // auto choices = audioParameter->getAllValueStrings();

  // if (parameter->textFromValueFunction)
  // slider->slider.textFromValueFunction = [parameter](double value) { return std::to_string(value * parameter.display_multiply); };
  // else if (choices.size() != 0) {
  //   slider->setType(InspectorSlider::Type::thumb);
  //   slider->slider.textFromValueFunction = [choices](double value) { return choices[value]; };
  // }

  // for (auto modulator : parameter->connections)
  //   slider->addModulationIndicator(modulator->magnitudeParameter->getValue(), modulator->source->colour.colour, static_cast<bool>(modulator->bipolarParameter->getValue()), parameter->audioParameter->getValue());

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