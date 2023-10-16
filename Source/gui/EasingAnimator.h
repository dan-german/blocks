#pragma once
#include <functional>
#include "gui/GraphicsTimer.h"
#include <juce_gui_basics/juce_gui_basics.h>

class EasingAnimator: private GraphicsTimer {
public:
  struct AnimateInput;
  void animate(AnimateInput input);
  void reset() {
    this->seconds = 0.0f;
    this->accumulator = 0.0f;
  }
private:
  std::function<void(float, float)> animationCallback = {};
  std::function<void()> animationCompletion = {};

  float accumulator = 0.0f;
  float seconds = 0.0f;
  juce::Path bezierPath;
  void update(const float secondsSinceLastUpdate) override;
};

struct EasingAnimator::AnimateInput {
  float seconds;
  std::function<void()> completion = {};
  std::function<void(float, float)> animation = {};
  float initialValue = 0.0f;
  float range[2] = { 0.0f, 1.0f };
  float points[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
};