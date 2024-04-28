#include "column_controls_container.h"
#include "settings/Constants.h"
#include "gui/GridComponent.h"
#include "gui/ThemeManager.h" 

ColumnControlsContainer::ColumnControlsContainer(BlocksSlider::Listener* listener) {
  for (int i = 0; i < Constants::columns; i++) {
    auto slider = createSlider("level", i, listener);
    slider->default_value_ = 1.0f;
    slider->juce_slider_.setValue(1.0f);
    slider->juce_slider_.setRange(0.0f, 1.0f);
    slider->parameter_name_ = "level";
    level_sliders_.push_back(std::move(slider));
  }

  for (int i = 0; i < Constants::columns; i++) {
    auto slider = createSlider("pan", i, listener);
    slider->juce_slider_.getProperties().set("isCenter", true);
    slider->juce_slider_.setRange(-1.0f, 1.0f);
    slider->default_value_ = 0.0f;
    slider->juce_slider_.setValue(0.0f);
    slider->parameter_name_ = "pan";
    pan_sliders_.push_back(std::move(slider));
  }

  // ThemeManager::shared()->addListener(this);
}

ColumnControlsContainer::~ColumnControlsContainer() {
  // ThemeManager::shared()->removeListener(this);
}

std::unique_ptr<BlocksSlider> ColumnControlsContainer::createSlider(std::string title, int column, BlocksSlider::Listener* listener) {
  auto slider = std::make_unique<BlocksSlider>(listener);
  slider->juce_slider_.textFromValueFunction = [title](double value) { return title; };
  slider->value_label_.setText(title, juce::dontSendNotification);
  slider->parameter_name_ = title;
  slider->module_id_ = { "column control", column + 1 };
  slider->module_id_.type = "column control";
  addAndMakeVisible(slider.get());
  return slider;
}

void ColumnControlsContainer::resized() {
  auto config = GridComponent::blocks_config;
  int vertical_margin = 2;
  auto slider_height = getHeight() / 2 - vertical_margin / 2;
  auto slider_width = config.itemWidth / 1.045f;
  for (int i = 0; i < Constants::columns; i++) {
    auto x = GridComponent::xForColumn(i, config);
    auto bounds = Rectangle<int>(x, 0, slider_width, slider_height);
    auto width_delta = config.itemWidth - slider_width;
    bounds = bounds.withWidth(slider_width).withX(bounds.getX() + width_delta / 2);
    level_sliders_[i]->setBounds(bounds);
    pan_sliders_[i]->setBounds(bounds.withY(slider_height + vertical_margin));
  }
}

void ColumnControlsContainer::paint(juce::Graphics& g) {
  // g.fillAll(juce::Colours::red);
}

// void ColumnControlsContainer::themeChanged(Theme theme) {
//   for (auto& slider : level_sliders_) {
//     slider->juce_slider_.setColour(Slider::ColourIds::trackColourId, theme.two);
//   }
//   for (auto& slider : pan_sliders_) {
//     slider->juce_slider_.setColour(Slider::ColourIds::trackColourId, theme.two);
//   }
// }

void ColumnControlsContainer::reset() {
  for (auto& slider : level_sliders_) { slider->juce_slider_.setValue(1.0f); }
  for (auto& slider : pan_sliders_) { slider->juce_slider_.setValue(0.0f); }
}

void ColumnControlsContainer::highlight(bool highlight, Colour color) {
  for (auto& slider : level_sliders_) { slider->setIndicationHighlight(highlight, color); }
  for (auto& slider : pan_sliders_) { slider->setIndicationHighlight(highlight, color); }
}