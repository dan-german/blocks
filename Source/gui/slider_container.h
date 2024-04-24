#include "controls/LabeledSlider.h"
namespace gui {
class SliderContainer: public juce::Component {
private:
  const int slider_width = 130;
  const int slider_height = 23;
  const int vertical_spacing = 5;
  const int max_columns = 3;
  const int topSpacing = 16;
  const int horizontal_spacing = 3;
  const int offset = 0;
  const int rows = 3;
public:
  SliderContainer() {
    // for (int i = 0; i < 2; i++) {
    //   auto slider = new LabeledSlider();
    //   sliders.push_back(slider);
    //   addAndMakeVisible(slider);
    // }
  }

  void paint(juce::Graphics& g) override {
    // g.fillAll(juce::Colours::blue);
  }

  void resized() override {
    resizeSliders();
  }

  int calculateWidth() {
    int columns = std::min(max_columns, sliders_.size());
    int combined_horizontal_spacing = (columns - 1) * horizontal_spacing;
    int combined_sliders_width = columns * slider_width;
    return combined_sliders_width + combined_horizontal_spacing;
  }

  int calculateHeight() {
    int rows = rowsForSliderCount(sliders_.size());
    int combined_vertical_spacing = (rows - 1) * vertical_spacing;
    int combined_sliders_height = rows * slider_height;
    return combined_sliders_height + combined_vertical_spacing;
  }

  int rowsForSliderCount(int slider_count) const {
    return (slider_count + max_columns - 1) / max_columns;
  }

  void resizeSliders() const {
    int rows = rowsForSliderCount(sliders_.size());
    int next_slider_index = 0;
    for (int row = 0; row < rows; row++) {
      for (int column = 0; column < max_columns; column++) {
        if (next_slider_index == sliders_.size()) return;
        auto slider = sliders_[next_slider_index++];
        int x = column * slider_width + horizontal_spacing;
        int y = row * slider_height + vertical_spacing * row;
        slider->setBounds(x, y, slider_width - horizontal_spacing * 2, slider_height);
      }
    }
  }

  void setModule(std::shared_ptr<model::Module> module) {
    sliders_.clear();
    for (auto parameter : module->parameters_) spawnSlider(*parameter, module);
    // updateSize();
  }

  void spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module) {
    if (parameter.hidden) return;
    auto slider = new InspectorSlider();
    double interval = 0.0;
    if (parameter.value_scale == ValueScale::kIndexed) {
      interval = 1.0;
    }
    slider->slider.setRange(parameter.min, parameter.max, interval);
    slider->slider.addListener(this);
    slider->slider.setNumDecimalPlacesToDisplay(parameter.decimal_places);
    slider->slider.textFromValueFunction = [this, parameter, module, slider](double value) {
      if (module->id.type == "delay") {
        onDelayAdjusted(module, parameter, value);
      } else if (module->id.type == "phaser" || module->id.type == "chorus" || module->id.type == "flanger") {
        bool is_changing_sync = parameter.name == "sync";
        if (is_changing_sync) {
          auto frequency_slider = getSliders()[3];
          bool is_seconds = int(value) == 0;
          if (is_seconds) {
            setSliderAsFrequency(module, frequency_slider, "frequency");
          } else {
            setSliderAsTempo(module, frequency_slider, "tempo");
          }
        }
      }

      return UIUtils::getSliderTextFromValue(value, parameter);
    };
    slider->titleLabel.setText(parameter.display_name, dontSendNotification);
    parameterSliders.add(slider);
    slider_to_parameter_name_map_[&slider->slider] = parameter.name;
    addAndMakeVisible(slider);
    auto value = parameter.value_processor->value();
    slider->slider.setValue(value, dontSendNotification);
  }

  std::vector<std::unique_ptr<LabeledSlider>> sliders_;
};
}