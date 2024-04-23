#include "column_controls_container.h"
#include "settings/Constants.h"
#include "gui/GridComponent.h"
#include "settings/GridConfigs.h"
#include "gui/ThemeManager.h" 

ColumnControlsContainer::ColumnControlsContainer() {
  for (int i = 0; i < Constants::columns; i++) {
    auto slider = createSlider("level", i);
    slider->default_value_ = 1.0f;
    slider->slider_.setValue(1.0f);
    slider->slider_.setRange(0.0f, 1.0f);
    level_sliders_.push_back(std::move(slider));
  }

  for (int i = 0; i < Constants::columns; i++) {
    auto slider = createSlider("pan", i);
    slider->slider_.getProperties().set("isCenter", true);
    slider->slider_.setRange(-1.0f, 1.0f);
    slider->default_value_ = 0.0f;
    slider->slider_.setValue(0.0f);
    pan_sliders_.push_back(std::move(slider));
  }

  ThemeManager::shared()->addListener(this);
}

ColumnControlsContainer::~ColumnControlsContainer() {
  ThemeManager::shared()->removeListener(this);
}

std::unique_ptr<BoxSlider> ColumnControlsContainer::createSlider(std::string title, int column) {
  auto slider = std::make_unique<BoxSlider>();
  slider->setAlpha(0.5f);
  slider->slider_.getProperties().set("tag", String(title));
  slider->slider_.getProperties().set("column", column);
  slider->slider_.textFromValueFunction = [title](double value) { return title; };
  slider->value_label_.setText(title, juce::dontSendNotification);
  slider->slider_.addListener(this);
  slider->parameter_name_ = title;
  slider->module_id_ = { "column control", column + 1 };
  addAndMakeVisible(slider.get());
  return slider;
}

void ColumnControlsContainer::resized() {
  auto config = GridConfigs::blocks;
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

void ColumnControlsContainer::themeChanged(Theme theme) {
  for (auto& slider : level_sliders_) {
    slider->slider_.setColour(Slider::ColourIds::trackColourId, theme.two.withAlpha(0.45f));
  }
  for (auto& slider : pan_sliders_) {
    slider->slider_.setColour(Slider::ColourIds::trackColourId, theme.two.withAlpha(0.45f));
  }
}

void ColumnControlsContainer::sliderValueChanged(Slider* slider) {
  auto column = slider->getProperties().getWithDefault("column", 0).toString().getIntValue();
  if (slider->getProperties().getWithDefault("tag", "") == "level") {
    listener->columnControlAdjusted(ControlType::level, column, slider->getValue());
  } else {
    listener->columnControlAdjusted(ControlType::pan, column, slider->getValue());
  }
}

void ColumnControlsContainer::sliderDragStarted(Slider* slider) {
  auto column = slider->getProperties().getWithDefault("column", 0).toString().getIntValue();
  if (slider->getProperties().getWithDefault("tag", "") == "level") {
    listener->columnControlStartedAdjusting(ControlType::level, column);
  } else {
    listener->columnControlStartedAdjusting(ControlType::pan, column);
  }
}

void ColumnControlsContainer::sliderDragEnded(Slider* slider) {
  auto column = slider->getProperties().getWithDefault("column", 0).toString().getIntValue();
  if (slider->getProperties().getWithDefault("tag", "") == "level") {
    listener->columnControlEndedAdjusting(ControlType::level, column);
  } else {
    listener->columnControlEndedAdjusting(ControlType::pan, column);
  }
}

void ColumnControlsContainer::reset() {
  for (auto& slider : level_sliders_) { slider->slider_.setValue(1.0f); }
  for (auto& slider : pan_sliders_) { slider->slider_.setValue(0.0f); }
}

void ColumnControlsContainer::highlight(bool highlight, Colour color) {
  for (auto& slider : level_sliders_) { slider->setIndicationHighlight(highlight, color); }
  for (auto& slider : pan_sliders_) { slider->setIndicationHighlight(highlight, color); }
}