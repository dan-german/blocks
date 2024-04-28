#include "gui/slider_container.h"
// #include "gui/controls/LabelledS"
#include "vital/synth_strings.h"

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
  resizeSliders();
}

void SliderContainer::sliderAdjusted(BlocksSlider* slider, float value) {
  if (slider->module_id_.type == "lfo") {
    onLFOAdjusted(slider);
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

  if (parameter.string_lookup) {
    labeled_slider->box_slider_.juce_slider_.textFromValueFunction = [parameter](double value) { return juce::String(parameter.string_lookup[(int)value]); };
  } else {
    labeled_slider->box_slider_.juce_slider_.textFromValueFunction = [parameter](double value) { return UIUtils::getSliderTextFromValue(value, parameter); };
  }

  labeled_slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  labeled_slider->box_slider_.value_label_.setText(labeled_slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);

  if (module->id.type == "lfo") {
    onLFOAdjusted(&labeled_slider->box_slider_);
  }

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

void SliderContainer::setSliderAsFrequency(LabeledSlider* slider) const {
  slider->label.setText("secs", dontSendNotification);

  auto frequency_parameter = module_->parameter_map_["frequency"];
  slider->box_slider_.juce_slider_.textFromValueFunction = [frequency_parameter](double value) {
    return UIUtils::getSliderTextFromValue(value, *frequency_parameter);
  };

  slider->box_slider_.juce_slider_.setRange(frequency_parameter->min, frequency_parameter->max);
  auto value = frequency_parameter->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}

void SliderContainer::setSliderAsTempo(LabeledSlider* slider) const {
  slider->label.setText("tempo", dontSendNotification);
  slider->box_slider_.juce_slider_.textFromValueFunction = [](double value) { return strings::kSyncedFrequencyNames[int(value)]; };
  slider->box_slider_.juce_slider_.setRange(0.0, 12.0, 1.0);
  auto value = module_->parameter_map_["tempo"]->value_processor->value();
  slider->box_slider_.juce_slider_.setValue(value, dontSendNotification);
  slider->box_slider_.value_label_.setText(slider->box_slider_.juce_slider_.getTextFromValue(value), dontSendNotification);
}