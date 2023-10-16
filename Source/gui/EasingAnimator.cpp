#include "gui/EasingAnimator.h"
#include <algorithm>

void EasingAnimator::update(const float deltaTime) {
  accumulator += deltaTime;

  if (accumulator >= seconds) {
    stop();
    this->seconds = 0.0f;
    this->accumulator = 0.0f;

    if (this->animationCompletion) {
      this->animationCompletion();
      this->animationCallback = nullptr;
    }

    return;
  }

  float progress = accumulator / seconds; // 0.0f to 1.0f
  auto y = bezierPath.getPointAlongPath(progress * bezierPath.getLength()).getY();
  animationCallback(y, progress);
}

void EasingAnimator::animate(AnimateInput input) {
  this->animationCallback = input.animation;
  this->animationCompletion = input.completion;
  this->seconds = input.seconds;

  juce::Point startingPoint(0.0f, input.range[0]);
  juce::Point endingPoint(0.0f, input.range[1]);

  bezierPath.clear();
  bezierPath.startNewSubPath(startingPoint);

  juce::Point<float> p1 = { input.points[0], input.points[1] };
  juce::Point<float> p2 = { input.points[2], input.points[3] };
  bezierPath.cubicTo(p1, p2, endingPoint);

  start();
}