#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/controls/blocks_slider.h"
#include "gui/ThemeListener.h"

class ColumnControlsContainer: public juce::Component {
public:
  enum class ControlType {
    level,
    pan
  };

  class Listener;
  Listener* listener;

  ColumnControlsContainer(BlocksSlider::Listener* blocks_slider_listener);
  ~ColumnControlsContainer();
  void paint(juce::Graphics&) override;
  float plusLineLength = 10;
  float plusLineWidth = 2;
  std::vector<std::unique_ptr<BlocksSlider>> level_sliders_;
  std::vector<std::unique_ptr<BlocksSlider>> pan_sliders_;
  void resized() override;
  void reset();
  void highlight(bool highlight, Colour color);
private:
  // void themeChanged(Theme theme) override;
  std::unique_ptr<BlocksSlider> createSlider(std::string title, int column, BlocksSlider::Listener* listener);
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColumnControlsContainer)
};

struct ColumnControlsContainer::Listener {
  virtual void columnControlAdjusted(ControlType control, int column, float value) = 0;
  virtual void columnControlStartedAdjusting(ControlType control, int column) = 0;
  virtual void columnControlEndedAdjusting(ControlType control, int column) = 0;
};