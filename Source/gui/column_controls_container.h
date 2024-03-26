#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/controls/BoxSlider.h"
#include "gui/ThemeListener.h"

class ColumnControlsContainer: public juce::Component, ThemeListener, Slider::Listener {
public:
  enum class ControlType {
    level,
    pan
  };

  class Listener;
  Listener* listener;

  ColumnControlsContainer();
  ~ColumnControlsContainer();
  void paint(juce::Graphics&) override;
  float plusLineLength = 10;
  float plusLineWidth = 2;
  std::vector<std::unique_ptr<BoxSlider>> level_sliders_;
  std::vector<std::unique_ptr<BoxSlider>> pan_sliders_;
  void resized() override;
  void reset();
  // Colour colour = Colour(66, 66, 66);
private:

  void sliderValueChanged(Slider* slider) override;
  void sliderDragStarted(Slider* slider) override;
  void sliderDragEnded(Slider* slider) override;

  void themeChanged(Theme theme) override;
  std::unique_ptr<BoxSlider> createSlider(std::string title, int column);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColumnControlsContainer)
};

struct ColumnControlsContainer::Listener {
  virtual void columnControlAdjusted(ControlType control, int column, float value) = 0;
  virtual void columnControlStartedAdjusting(ControlType control, int column) = 0;
  virtual void columnControlEndedAdjusting(ControlType control, int column) = 0;
};