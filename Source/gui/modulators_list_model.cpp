/*
  ==============================================================================

    ModulatorsListModel.cpp
    Created: 30 Oct 2021 12:29:38am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/modulators_list_model.h"
#include "model/lfo_model.h"
#include "module_new.h"
#include "ui_utils.h"

int ModulatorsListModel::getNumRows() { return modulators_.size(); }
void ModulatorsListModel::listBoxItemDoubleClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemDoubleClicked(row, event); }
var ModulatorsListModel::getDragSourceDescription(const SparseSet<int>& rowsToDescribe) { return ListBoxModel::getDragSourceDescription(rowsToDescribe); }
void ModulatorsListModel::remove(int index) {
  modulators_.erase(modulators_.begin() + index);
}

Component* ModulatorsListModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  ModulatorComponent* component;

  if (auto castComponent = static_cast<ModulatorComponent*>(existingComponentToUpdate))
    component = castComponent;
  else
    component = new ModulatorComponent(this);

  component->onSliderValueChange = {};
  if (modulators_.size() == 0) return component;
  if (rowNumber >= modulators_.size()) return component;

  component->row = rowNumber;
  auto model = modulators_[rowNumber];
  setModelToComponent(model, *component);
  component->slider_container_.setModule(model);
  modulator_component_map_[model->id.getName()] = component;

  return component;
}

void ModulatorsListModel::sliderAdjusted(BlocksSlider* slider, float value) {
  auto model = model_map_[slider->module_id_.getName()];
  if (slider->module_id_.type == "lfo") {
    onLFOAdjusted(model, slider->parameter_name_, value);
  } else if (slider->module_id_.type == "envelope") {
    onEnvelopeAdjusted(model, slider->parameter_name_, value);
  }
}

void ModulatorsListModel::setModelToComponent(std::shared_ptr<model::Module> model, ModulatorComponent& component) const {
  component.title.setText(model->display_name, dontSendNotification);
  component.delegate_ = modulator_listener;
  component.setColour(model->colour.colour);
  if (model->id.type == "envelope") {
    component.oscillatorPainter.setVisible(false);
    component.envelopePath.setVisible(true);
    // onEnvelopeParameterChanged(value, model, i, component);
  } else {
    component.oscillatorPainter.setVisible(true);
    component.envelopePath.setVisible(false);
    if (model->id.type == "random") component.oscillatorPainter.setWaveformType(OscillatorPainter::WaveformType::noise);
  }
}

void ModulatorsListModel::onEnvelopeAdjusted(std::shared_ptr<model::Module> model, std::string parameter_name, float value) const {
  auto parameter = model->parameter_map_[parameter_name];
  auto normalized_value = juce::jmap(value, parameter->min, parameter->max, 0.0f, 1.0f);
  auto modulator_component = modulator_component_map_.at(model->id.getName());
  if (parameter_name == "attack") { 
    modulator_component->envelopePath.setAttack(normalized_value);
  } else if (parameter_name == "decay") {
    modulator_component->envelopePath.setDecay(normalized_value);
  } else if (parameter_name == "sustain") {
    modulator_component->envelopePath.setSustain(normalized_value);
  } else if (parameter_name == "release") {
    modulator_component->envelopePath.setRelease(normalized_value);
  }

  // switch (index) {
  // case 0: component.envelopePath.setAttack(normalized_value); break;
  // case 1: component.envelopePath.setDecay(normalized_value); break;
  // case 2: component.envelopePath.setSustain(normalized_value); break;
  // case 3: component.envelopePath.setRelease(normalized_value); break;
  // default: break;
  // }
}

void ModulatorsListModel::setModulators(std::vector<std::shared_ptr<model::Module>> modulators) {
  modulators_.clear();
  modulators_ = modulators;
  model_map_.clear();
  for (auto modulator : modulators) {
    model_map_[modulator->id.getName()] = modulator;
  }
  modulator_component_map_.clear();
}

void ModulatorsListModel::onLFOAdjusted(std::shared_ptr<model::Module> module, std::string parameter_name, float value) const {
  if (parameter_name == "wave") {
    auto component = modulator_component_map_.at(module->id.getName());
    component->oscillatorPainter.waveformType = static_cast<OscillatorPainter::WaveformType>(int(value));
  }

  // bool is_changing_tempo = index == 1;
  // bool is_not_seconds = int(component.sliders[2]->box_slider_.juce_slider_.getValue()) != 0;
  // if (is_changing_tempo && is_not_seconds) {
  //   auto value = component.sliders[1]->box_slider_.juce_slider_.getValue();
  //   std::string value_string = std::to_string(value);
  //   auto integer_part_length = value_string.substr(0, value_string.find(".")).size();
  //   component.sliders[1]->box_slider_.juce_slider_.setNumDecimalPlacesToDisplay(4 - integer_part_length);
  // }
}

void ModulatorsListModel::setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("secs", dontSendNotification);
  auto frequency_parameter = module->parameter_map_["frequency"];
  slider->box_slider_.juce_slider_.textFromValueFunction = [frequency_parameter, module](double value) {
    return UIUtils::getSliderTextFromValue(value, *frequency_parameter);
  };
  slider->box_slider_.juce_slider_.setRange(frequency_parameter->min, frequency_parameter->max);
  auto value = frequency_parameter->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}

void ModulatorsListModel::setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("tempo", dontSendNotification);
  slider->box_slider_.juce_slider_.textFromValueFunction = [](double value) { return strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.juce_slider_.setRange(0.0, 12.0, 1.0);
  auto value = module->parameter_map_["tempo"]->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}