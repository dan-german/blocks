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

int ModulatorsListModel::getNumRows() { return modulators.size(); }
void ModulatorsListModel::listBoxItemDoubleClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemDoubleClicked(row, event); }
var ModulatorsListModel::getDragSourceDescription(const SparseSet<int>& rowsToDescribe) { return ListBoxModel::getDragSourceDescription(rowsToDescribe); }
void ModulatorsListModel::remove(int index) {
  modulators.erase(modulators.begin() + index);
}

Component* ModulatorsListModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  ModulatorComponent* component;

  if (auto castComponent = static_cast<ModulatorComponent*>(existingComponentToUpdate))
    component = castComponent;
  else
    component = new ModulatorComponent();

  component->onSliderValueChange = {};
  if (modulators.size() == 0) return component;
  if (rowNumber >= modulators.size()) return component;

  component->row = rowNumber;
  component->slider_container_.setModule(modulators[rowNumber]);

  return component;
}

void ModulatorsListModel::setupModulatorComponent(std::shared_ptr<model::Module> model, ModulatorComponent& component) const {
  component.title.setText(model->display_name, dontSendNotification);
  component.delegate_ = modulator_listener;
  component.setColour(model->colour.colour);

  if (model->id.type == "envelope") {
    component.oscillatorPainter.setVisible(false);
    component.envelopePath.setVisible(true);
  } else {
    component.oscillatorPainter.setVisible(true);
    component.envelopePath.setVisible(false);
    if (model->id.type == "random") component.oscillatorPainter.setWaveformType(OscillatorPainter::WaveformType::noise);
  }

  if (model->id.type == "envelope") {
    component.onSliderValueChange = [&component, model, this](int index, float value) {
      this->onEnvelopeParameterChanged(value, model, index, component);
    };
  } else {
    component.onSliderValueChange = [model, &component, this](int index, float value) {
      this->onLFOParameterChange(model, component, index, value);
    };
  }

  for (int i = 0; i < model->parameters_.size(); i++) {
    auto parameter = model->parameters_[i];
    if (parameter->hidden) continue;
    auto labeled_slider = component.sliders[i];
    component.slider_parameter_name_map_[&labeled_slider->box_slider_.slider_] = parameter->name;

    auto value = parameter->value_processor->value();
    labeled_slider->label.setText(parameter->display_name, dontSendNotification);
    labeled_slider->box_slider_.modulatable = parameter->modulatable;
    labeled_slider->box_slider_.module_id_ = model->id;
    labeled_slider->box_slider_.parameter_name_ = parameter->name;

    double interval = 0.0;
    if (parameter->value_scale == ValueScale::kLinear) {
      labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(3);
    } else if (parameter->value_scale == ValueScale::kExponential) {
      // slider->boxSlider.slider.setSkewFactor(4, false);
    } else if (parameter->value_scale == ValueScale::kIndexed) {
      interval = 1.0;
      labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(0);
    }

    labeled_slider->box_slider_.slider_.setRange(parameter->min, parameter->max, interval);

    if (parameter->string_lookup) {
      labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return juce::String(parameter->string_lookup[(int)value]); };
    } else {
      labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, *parameter); };
    }

    labeled_slider->box_slider_.slider_.setValue(value, dontSendNotification);
    labeled_slider->box_slider_.value_label_.setText(labeled_slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);

    if (model->id.type == "envelope") {
      onEnvelopeParameterChanged(value, model, i, component);
    } else {
      onLFOParameterChange(model, component, i, value);
    }
  }
}

void ModulatorsListModel::onEnvelopeParameterChanged(float value, std::shared_ptr<model::Module> model, int index, ModulatorComponent& component) const {
  auto parameter = model->parameters_[index];
  auto normalized_value = juce::jmap(value, parameter->min, parameter->max, 0.0f, 1.0f);
  switch (index) {
  case 0: component.envelopePath.setAttack(normalized_value); break;
  case 1: component.envelopePath.setDecay(normalized_value); break;
  case 2: component.envelopePath.setSustain(normalized_value); break;
  case 3: component.envelopePath.setRelease(normalized_value); break;
  default: break;
  }
}

void ModulatorsListModel::setModulators(std::vector<std::shared_ptr<model::Module>> modulators) {
  this->modulators.clear();
  this->modulators = modulators;
}

void ModulatorsListModel::onLFOParameterChange(std::shared_ptr<model::Module> module, ModulatorComponent& component, int index, float value) const {
  bool is_changing_wave = index == 0;
  bool is_changing_sync = index == 2;
  if (is_changing_sync) {
    LabeledSlider* rate_slider = component.sliders[1];
    bool is_seconds = int(value) == 0;
    if (is_seconds) {
      setSliderAsFrequency(module, rate_slider);
    } else {
      setSliderAsTempo(module, rate_slider);
    }
  } else if (is_changing_wave && module->id.type != "random") {
    auto wave = int(value);
    component.oscillatorPainter.waveformType = static_cast<OscillatorPainter::WaveformType>(wave);
  }

  bool is_changing_tempo = index == 1;
  bool is_not_seconds = int(component.sliders[2]->box_slider_.slider_.getValue()) != 0;
  if (is_changing_tempo && is_not_seconds) {
    auto value = component.sliders[1]->box_slider_.slider_.getValue();
    std::string value_string = std::to_string(value);
    auto integer_part_length = value_string.substr(0, value_string.find(".")).size();
    component.sliders[1]->box_slider_.slider_.setNumDecimalPlacesToDisplay(4 - integer_part_length);
  }
}

void ModulatorsListModel::setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("secs", dontSendNotification);

  auto frequency_parameter = module->parameter_map_["frequency"];
  slider->box_slider_.slider_.textFromValueFunction = [frequency_parameter, module](double value) {
    return UIUtils::getSliderTextFromValue(value, *frequency_parameter);
  };

  slider->box_slider_.slider_.setRange(frequency_parameter->min, frequency_parameter->max);
  auto value = frequency_parameter->value_processor->value();
  slider->box_slider_.slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);
}

void ModulatorsListModel::setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("tempo", dontSendNotification);
  slider->box_slider_.slider_.textFromValueFunction = [](double value) { return strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.slider_.setRange(0.0, 12.0, 1.0);
  auto value = module->parameter_map_["tempo"]->value_processor->value();
  slider->box_slider_.slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);
}