#pragma once

class ModulationIndicator: public juce::Component {
public:
  class IndicatorModel;
  ModulationIndicator() { }

  Colour colour;
  DrawablePath line_;
  Component* clip_component_;

  // std::vector<IndicatorModel> indicator_models_;

  void resized() override {
    Path p;
    p.addRectangle(getLocalBounds());
    line_.setPath(p);
  }

  Path path;
  float corner_radius_ = 5.0f;

  void paint(juce::Graphics& g) override {
    const juce::Graphics::ScopedSaveState save(g);
    auto parentRect = getLocalArea(clip_component_, clip_component_->getLocalBounds());
    path.clear();
    path.addRoundedRectangle(parentRect.toFloat(), corner_radius_);
    g.reduceClipRegion(path);

    Colour colors[] = {
      Colour(125, 255, 136),
      Colour(94, 173, 247),
      Colour(255, 99, 144)
    };

    int height = 1;
    for (int i = 0; i < 3; i++) {
      g.setColour(colors[i].withAlpha(0.5f));
      int y = parentRect.getHeight() - height * (i + 1);
      int width = getWidth() / 2 * (i + 1);
      g.fillRect(getWidth() / 2, y, width, height);
    }
    // int height = getHeight() / 6;
    // g.fillRect(getWidth() / 2, parentRect.getHeight() - height, getHeight() / 2, height);
  }
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationIndicator)
};

// class ModulationIndicator::IndicatorModel {
//   bool bipolar_;
//   float value_;
// };