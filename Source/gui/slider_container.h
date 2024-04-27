#pragma once
#include "controls/LabeledSlider.h"
#include "ui_utils.h"
#include "module_new.h"

namespace gui {
class SliderContainer: public juce::Component, juce::Slider::Listener {
private:
  const int slider_width = 130;
  const int slider_height = 23;
  const int vertical_spacing = 5;
  const int topSpacing = 16;
  const int horizontal_spacing = 3;
  const int offset = 0;
  int max_columns_ = 3;
  BlocksSlider::Listener* slider_listener_;
public:
  struct Config {
    int max_columns;
    int slider_width;
    int slider_height;
  };
  Config config_;

  inline static Config inspector_config = { .max_columns = 3, .slider_width = 130, .slider_height = 23 };
  inline static Config modulator_config = { .max_columns = 2, .slider_width = 130, .slider_height = 23 };

  SliderContainer(BlocksSlider::Listener* slider_listener, Config& config);

  void paint(juce::Graphics& g) override;
  void resized() override;
  int calculateWidth();
  int calculateHeight();

  void sliderValueChanged(juce::Slider* slider) override;

  int rowsForSliderCount(int slider_count) const;

  void onEnvelopeParameterChanged(float value, std::shared_ptr<model::Module> model, int index) const;
  void onLFOParameterChange(std::shared_ptr<model::Module> module, int index, float value) const;
  void setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const;
  void setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const;

  void resizeSliders() const;
  void setModule(std::shared_ptr<model::Module> module);
  void spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module);
  std::vector<std::unique_ptr<LabeledSlider>> sliders_;
  std::unordered_map<std::string, LabeledSlider*> slider_map_;
};


} // namespace gui
