#include "controls/LabeledSlider.h"
#include "ui_utils.h"
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
  BlocksSlider::Listener* slider_listener_;
public:

  SliderContainer(BlocksSlider::Listener* slider_listener): slider_listener_(slider_listener) { }

  // void paint(juce::Graphics& g) override { }

  void resized() override {
    resizeSliders();
  }

  int calculateWidth() {
    int columns = std::min(max_columns, (int)sliders_.size());
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
        int x = column * slider_width + horizontal_spacing;
        int y = row * slider_height + vertical_spacing * row;
        sliders_[next_slider_index++]->setBounds(x, y, slider_width - horizontal_spacing * 2, slider_height);
      }
    }
  }

  void setModule(std::shared_ptr<model::Module> module) {
    // for (auto slider : sliders) slider->removeListener(this);
    sliders_.clear();
    for (auto parameter : module->parameters_) spawnSlider(*parameter, module);
    resizeSliders();
  }

  void spawnSlider(vital::ValueDetails parameter, std::shared_ptr<model::Module> module) {
    if (parameter.hidden) return;
    auto labeled_slider = std::make_unique<LabeledSlider>(slider_listener_);
    addAndMakeVisible(labeled_slider.get());

    auto value = parameter.value_processor->value();
    labeled_slider->label.setText(parameter.display_name, dontSendNotification);
    labeled_slider->box_slider_.modulatable = parameter.modulatable;
    labeled_slider->box_slider_.module_id_ = module->id;
    labeled_slider->box_slider_.parameter_name_ = parameter.name;

    double interval = 0.0;
    if (parameter.value_scale == ValueScale::kLinear) {
      labeled_slider->box_slider_.slider_.setNumDecimalPlacesToDisplay(3);
    } else if (parameter.value_scale == ValueScale::kExponential) {
      // slider->boxSlider.slider.setSkewFactor(4, false);
    } else if (parameter.value_scale == ValueScale::kIndexed) {
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

    sliders_.push_back(std::move(labeled_slider));
  }

  // void BlocksSlider::sliderValueChanged(Slider* slider) {
  //   float value = static_cast<float>(slider->getValue());
  //   if (onSliderValueChange) onSliderValueChange(currentSliderIndex, value);
  //   auto name = slider_parameter_name_map_[slider];
  //   delegate_->modulatorIsAdjusting(this, name, value);
  // }

  std::vector<std::unique_ptr<LabeledSlider>> sliders_;
};
} // namespace gui
