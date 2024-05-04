#include "gui/slider_container.h"
#include "vital/synth_strings.h"

using namespace gui;

SliderContainer::SliderContainer(BlocksSlider::Listener* slider_listener, Config& config):
  slider_listener_(slider_listener),
  config_(config)
{}

void SliderContainer::paint(juce::Graphics& g) {
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
  module_ = module;
  sliders_.clear();
  for (auto parameter : module->parameters_) spawnSlider(*parameter, module);
  for (auto& slider : sliders_) sliderAdjusted(&slider->box_slider_, slider->box_slider_.juce_slider_.getValue());
  resizeSliders();
}

void SliderContainer::sliderAdjusted(BlocksSlider* slider, float value) {
  auto model_type = slider->module_id_.type;
  if (model_type == "lfo") {
    onLFOAdjusted(slider);
  } else if (model_type == "delay") {
    onDelayAdjusted(slider);
  } else if (model_type == "phaser" || model_type == "chorus" || model_type == "flanger") {
    bool is_changing_sync = slider->parameter_name_ == "sync";
    if (is_changing_sync) {
      auto tempo_slider = slider_map_.at("tempo");
      bool is_seconds = int(value) == 0;
      if (is_seconds) {
        setSliderAsFrequency(tempo_slider);
      } else {
        setSliderAsTempo(tempo_slider);
      }
    }
  }
}

void SliderContainer::spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module) {
  if (parameter.hidden) return;
  auto labeled_slider = std::make_unique<LabeledSlider>(slider_listener_);
  labeled_slider->box_slider_.addListener(this);
  addAndMakeVisible(labeled_slider.get());
  slider_map_[parameter.name] = labeled_slider.get();

  auto value = parameter.value_processor->value();
  labeled_slider->label.setText(parameter.display_name, dontSendNotification);
  labeled_slider->box_slider_.modulatable = parameter.modulatable;
  labeled_slider->box_slider_.module_id_ = module->id;
  labeled_slider->box_slider_.parameter_name_ = parameter.name;

  double interval = 0.0;
  if (parameter.value_scale == vital::ValueDetails::kLinear) {
    labeled_slider->box_slider_.juce_slider_.setNumDecimalPlacesToDisplay(3);
  } else if (parameter.value_scale == vital::ValueDetails::ValueScale::kIndexed) {
    interval = 1.0;
    labeled_slider->box_slider_.juce_slider_.setNumDecimalPlacesToDisplay(0);
  }

  labeled_slider->box_slider_.juce_slider_.setRange(parameter.min, parameter.max, interval);

  bool centered = false;
  if (parameter.name == "tempo") {
    centered = false;
  } else if (parameter.min < 0) {
    centered = true;
  }
  labeled_slider->box_slider_.juce_slider_.getProperties().set("isCenter", centered);

  if (parameter.string_lookup) {
    labeled_slider->box_slider_.juce_slider_.textFromValueFunction = [parameter](double value) { return juce::String(parameter.string_lookup[(int)value]); };
  } else {
    labeled_slider->box_slider_.juce_slider_.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, parameter); };
  }

  labeled_slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  labeled_slider->box_slider_.value_label_.setText(labeled_slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
  sliders_.push_back(std::move(labeled_slider));
}

void SliderContainer::onLFOAdjusted(BlocksSlider* slider) const {
  bool is_changing_wave = slider->parameter_name_ == "wave";
  bool is_changing_sync = slider->parameter_name_ == "sync";
  if (is_changing_sync) {
    auto rate_slider = slider_map_.at("tempo");
    bool is_seconds = int(slider->juce_slider_.getValue()) == 0;
    if (is_seconds) {
      setSliderAsFrequency(rate_slider);
    } else {
      setSliderAsTempo(rate_slider);
    }
  }
}

void SliderContainer::setSliderAsFrequency(LabeledSlider* slider, std::string parameter_name) const {
  slider->label.setText("secs", dontSendNotification);

  auto frequency_parameter = module_->parameter_map_[parameter_name];
  slider->box_slider_.juce_slider_.textFromValueFunction = [frequency_parameter](double value) {
    return UIUtils::getSliderTextFromValue(value, *frequency_parameter);
  };

  slider->box_slider_.juce_slider_.setRange(frequency_parameter->min, frequency_parameter->max);
  auto value = frequency_parameter->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}

void SliderContainer::setSliderAsTempo(LabeledSlider* slider, std::string parameter_name) const {
  slider->label.setText("tempo", dontSendNotification);
  slider->box_slider_.juce_slider_.textFromValueFunction = [](double value) { return strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.juce_slider_.setRange(0.0, 12.0, 1.0);
  auto value = module_->parameter_map_[parameter_name]->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}

void SliderContainer::setSlidersColour(Colour& colour) {
  for (auto& slider : sliders_) {
    slider->box_slider_.custom_track_colour_ = true;
    slider->box_slider_.juce_slider_.setColour(Slider::ColourIds::trackColourId, colour.darker(0.9f));
  }
}

void SliderContainer::addSliderListener(BlocksSlider::Listener* listener) {
  for (auto& slider : sliders_) {
    slider->box_slider_.addListener(listener);
  }
}

void SliderContainer::highlightModulationIndication(bool should_highlight, Colour colour) {
  for (auto& slider : sliders_) {
    slider->box_slider_.setIndicationHighlight(should_highlight, colour);
  }
}

void SliderContainer::onDelayAdjusted(BlocksSlider* slider) const {
  bool is_changing_sync_1 = slider->parameter_name_ == "sync";
  bool is_changing_sync_2 = slider->parameter_name_ == "sync 2";
  if (is_changing_sync_1 || is_changing_sync_2) {
    std::string tempo_name = is_changing_sync_1 ? "tempo" : "tempo 2";
    auto tempo_slider = slider_map_.at(tempo_name);
    bool is_seconds = int(slider->juce_slider_.getValue()) == 0;
    if (is_seconds) {
      std::string frequency_name = is_changing_sync_1 ? "frequency" : "frequency 2";
      setSliderAsFrequency(tempo_slider, frequency_name);
    } else {
      setSliderAsTempo(tempo_slider, tempo_name);
    }
  }
}