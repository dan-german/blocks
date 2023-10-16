#include "gui/base/BaseButton.h"
#include "gui/OscillatorPainter.h"

class ModulatorsButton: public BaseButton {
public:
  ModulatorsButton() {
    addAndMakeVisible(painter);
    painter.setWaveformType(OscillatorPainter::WaveformType::sine);
    painter.speed = 10.0f;
    painter.waveColour = Colour(100, 100, 100);
    painter.cycles = 1.5f;
    painter.thickness = 2.25f;
  }

  void startSelectedAnimation() override {
    auto bounds = getContent()->getBounds();

    auto animation = [this, bounds](float value, float progress) {
      setButtonColour(colour.brighter(jmap(progress, 0.0f, 1.0f, 0.0f, hoverBrightness)));
    };

    auto completion = [this, bounds]() {
      isMouseHovering = true;
      setButtonColour(colour.brighter(hoverBrightness));
    };

    EasingAnimator::AnimateInput input = {
      .seconds = 0.06f,
      .completion = completion,
      .animation = animation,
      .range = { 0.95f, 1.0f },
      .points = { 0.50f, 0.75f, 0.30f, 1.20f },
    };

    animator.animate(input);
  }

  juce::Component* getContent() override { return &painter; }
  void setButtonColour(Colour colour) override { painter.waveColour = colour; }
private:
  OscillatorPainter painter;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulatorsButton)
};