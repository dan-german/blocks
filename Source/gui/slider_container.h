#pragma once
#include "controls/LabeledSlider.h"
#include "ui_utils.h"
#include "module_new.h"

namespace gui {
class SliderContainer: public juce::Component, BlocksSlider::Listener {
private:
  const int slider_width = 130;
  const int slider_height = 23;
  const int vertical_spacing = 5;
  const int topSpacing = 16;
  const int horizontal_spacing = 3;
  const int offset = 0;
  int max_columns_ = 3;
  BlocksSlider::Listener* slider_listener_;
  std::shared_ptr<model::Module> module_;

  void sliderAdjusted(BlocksSlider* slider, float value) override;
public:
  struct Config {
    int max_columns;
    int slider_width;
    int slider_height;
  };
  Config config_;

  inline static Config inspector_config = { .max_columns = 3, .slider_width = 130, .slider_height = 23 };
  inline static Config modulator_config = { .max_columns = 2, .slider_width = 130, .slider_height = 23 };

  std::vector<std::unique_ptr<LabeledSlider>> sliders_;
  std::unordered_map<std::string, LabeledSlider*> slider_map_;

  SliderContainer(BlocksSlider::Listener* slider_listener, Config& config);
  void paint(juce::Graphics& g) override;
  void resized() override;
  int calculateWidth();
  int calculateHeight();
  int rowsForSliderCount(int slider_count) const;
  void onDelayAdjusted(BlocksSlider* slider) const;
  void onLFOAdjusted(BlocksSlider* slider) const;
  void setSliderAsTempo(LabeledSlider* slider, std::string parameter_name = "tempo") const;
  void setSliderAsFrequency(LabeledSlider* slider, std::string parameter_name = "frequency") const;
  void resizeSliders() const;
  void setModule(std::shared_ptr<model::Module> module);
  void spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module);
  void setSlidersColour(Colour& colour);
  void addSliderListener(BlocksSlider::Listener* listener);
  void highlightModulationIndication(bool should_highlight, Colour colour);
};
} // namespace gui
