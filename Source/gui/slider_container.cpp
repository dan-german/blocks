#include "gui/slider_container.h"
// #include "gui/controls/LabelledS"

using namespace gui;

SliderContainer::SliderContainer(BlocksSlider::Listener* slider_listener, Config& config):
  slider_listener_(slider_listener),
  config_(config)
{}

void SliderContainer::paint(juce::Graphics& g) {
  // g.fillAll(juce::Colours::red);
}

void SliderContainer::resized() {
  resizeSliders();
}

int SliderContainer::calculateWidth() {
  int columns = std::min(config_.max_columns, static_cast<int>(sliders_.size()));
  int combined_horizontal_spacing = (columns - 1) * horizontal_spacing;
  int combined_sliders_width = columns * config_.slider_width;
  return combined_sliders_width + combined_horizontal_spacing;
}

int SliderContainer::calculateHeight() {
  int rows = rowsForSliderCount(sliders_.size());
  int combined_vertical_spacing = (rows - 1) * vertical_spacing;
  int combined_sliders_height = rows * slider_height;
  return combined_sliders_height + combined_vertical_spacing;
}

int SliderContainer::rowsForSliderCount(int slider_count) const {
  return (slider_count + config_.max_columns - 1) / config_.max_columns;
}

void SliderContainer::resizeSliders() const {
  // printf("max_columns_: %d\n", config_.max_columns);
  int rows = rowsForSliderCount(sliders_.size());
  int next_slider_index = 0;
  for (int row = 0; row < rows; row++) {
    for (int column = 0; column < config_.max_columns; column++) {
      if (next_slider_index == sliders_.size()) return;
      int x = column * config_.slider_width + horizontal_spacing * column;
      int y = row * slider_height + vertical_spacing * row;
      sliders_[next_slider_index++]->setBounds(x, y, config_.slider_width - horizontal_spacing * 2, slider_height);
    }
  }
}

void SliderContainer::setModule(std::shared_ptr<model::Module> module) {
  sliders_.clear();
  for (auto parameter : module->parameters_) spawnSlider(*parameter, module);
  resizeSliders();
}

void SliderContainer::sliderValueChanged(juce::Slider* slider) {
  // if (
  // if (module->id
}

void SliderContainer::spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module) {
  if (parameter.hidden) return;
  auto labeled_slider = std::make_unique<LabeledSlider>(slider_listener_);
  addAndMakeVisible(labeled_slider.get());
  slider_map_[parameter.name] = labeled_slider.get();

  // if (module->id.type == "envelope") {
    // component.onSliderValueChange = [&component, model, this](int index, float value) {
    //   this->onEnvelopeParameterChanged(value, model, index, component);
    // };
  // } else if (module->id.type == "lfo") {
  //   // labeled_slider.
  //   labeled_slider->box_slider_.slider_.onsli
  //   slider->slider.onSliderValueChange = [model, this](int index, float value) {
  //     this->onLFOParameterChange(model, index, value);
  //   };
  // }

  auto value = parameter.value_processor->value();
  labeled_slider->label.setText(parameter.display_name, dontSendNotification);
  labeled_slider->box_slider_.modulatable = parameter.modulatable;
  labeled_slider->box_slider_.module_id_ = module->id;
  labeled_slider->box_slider_.parameter_name_ = parameter.name;

  double interval = 0.0;
  if (parameter.value_scale == vital::ValueDetails::kLinear) {
    labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(3);
  } else if (parameter.value_scale == vital::ValueDetails::ValueScale::kIndexed) {
    interval = 1.0;
    labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(0);
  }

  labeled_slider->box_slider_.slider_.setRange(parameter.min, parameter.max, interval);

  if (parameter.string_lookup) {
    labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return juce::String(parameter.string_lookup[(int)value]); };
  } else {
    labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, parameter); };
  }

  labeled_slider->box_slider_.slider_.setValue(value, dontSendNotification);
  labeled_slider->box_slider_.value_label_.setText(labeled_slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);

  // if (module->id.type == "envelope") {
  //   onEnvelopeParameterChanged(value, module, i, component);
  // } else {
  //   onLFOParameterChange(module, component, i, value);
  // }

  sliders_.push_back(std::move(labeled_slider));
}

// void SliderContainer::onEnvelopeParameterChanged(float value, std::shared_ptr<model::Module> model, int index, ModulatorComponent& component) const {
//   auto parameter = model->parameters_[index];
//   auto normalized_value = juce::jmap(value, parameter->min, parameter->max, 0.0f, 1.0f);
//   switch (index) {
//   case 0: component.envelopePath.setAttack(normalized_value); break;
//   case 1: component.envelopePath.setDecay(normalized_value); break;
//   case 2: component.envelopePath.setSustain(normalized_value); break;
//   case 3: component.envelopePath.setRelease(normalized_value); break;
//   default: break;
//   }
// }

void SliderContainer::onLFOParameterChange(std::shared_ptr<model::Module> module, int index, float value) const {
  bool is_changing_wave = index == 0;
  bool is_changing_sync = index == 2;
  if (is_changing_sync) {
    // LabeledSlider* rate_slider = slider_map_["tempo"];
    // LabeledSlider* rate_slider = component.sliders[1];
    bool is_seconds = int(value) == 0;
    if (is_seconds) {
      // setSliderAsFrequency(module, rate_slider);
    } else {
      // setSliderAsTempo(module, rate_slider);
    }
  } else if (is_changing_wave && module->id.type != "random") {
    // auto wave = int(value);
    // component.oscillatorPainter.waveformType = static_cast<OscillatorPainter::WaveformType>(wave);
  }

  // bool is_changing_tempo = index == 1;
  // bool is_not_seconds = int(component.sliders[2]->box_slider_.slider_.getValue()) != 0;
  // if (is_changing_tempo && is_not_seconds) {
  //   auto value = component.sliders[1]->box_slider_.slider_.getValue();
  //   std::string value_string = std::to_string(value);
  //   auto integer_part_length = value_string.substr(0, value_string.find(".")).size();
  //   component.sliders[1]->box_slider_.slider_.setNumDecimalPlacesToDisplay(4 - integer_part_length);
  // }
}

void SliderContainer::setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
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

void SliderContainer::setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const {
  slider->label.setText("tempo", dontSendNotification);
  // slider->box_slider_.slider_.textFromValueFunction = [](double value) { return vital::strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.slider_.setRange(0.0, 12.0, 1.0);
  auto value = module->parameter_map_["tempo"]->value_processor->value();
  slider->box_slider_.slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);
}

// auto parameter = model->parameters_[i];
// if (parameter->hidden) continue;
// auto labeled_slider = component.sliders[i];
// component.slider_parameter_name_map_[&labeled_slider->box_slider_.slider_] = parameter->name;

// auto value = parameter->value_processor->value();
// labeled_slider->label.setText(parameter->display_name, dontSendNotification);
// labeled_slider->box_slider_.modulatable = parameter->modulatable;
// labeled_slider->box_slider_.module_id_ = model->id;
// labeled_slider->box_slider_.parameter_name_ = parameter->name;

// double interval = 0.0;
// if (parameter->value_scale == ValueScale::kLinear) {
//   labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(3);
// } else if (parameter->value_scale == ValueScale::kExponential) {
//   // slider->boxSlider.slider.setSkewFactor(4, false);
// } else if (parameter->value_scale == ValueScale::kIndexed) {
//   interval = 1.0;
//   labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(0);
// }

// labeled_slider->box_slider_.slider_.setRange(parameter->min, parameter->max, interval);

// if (parameter->string_lookup) {
//   labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return juce::String(parameter->string_lookup[(int)value]); };
// } else {
//   labeled_slider->box_slider_.slider_.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, *parameter); };
// }

// labeled_slider->box_slider_.slider_.setValue(value, dontSendNotification);
// labeled_slider->box_slider_.value_label_.setText(labeled_slider->box_slider_.slider_.getTextFromValue(value), dontSendNotification);

// if (model->id.type == "envelope") {
//   onEnvelopeParameterChanged(value, model, i, component);
// } else {
//   onLFOParameterChange(model, component, i, value);
// }