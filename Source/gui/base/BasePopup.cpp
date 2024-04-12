#include "gui/base/BasePopup.h"

#include "gui/ThemeManager.h"

void BasePopup::paint(juce::Graphics& g) {
  juce::Path path;
  path.addRoundedRectangle(getLocalBounds(), cornerRadius);
  g.reduceClipRegion(path);
  setBackgroundColour(ThemeManager::shared()->getCurrent().background.brighter(0.15f));
  g.fillAll(ThemeManager::shared()->getCurrent().background.brighter(0.15f));
}

void BasePopup::setVisible(bool shouldBeVisible) {
  if (!shouldBeVisible) {
    setInterceptsMouseClicks(false, false);
    triggerDismissAnimation();
  } else {
    juce::Component::setVisible(true);
  }
}

void BasePopup::triggerDismissAnimation() {
  auto bounds = getBounds();
  auto animation = [this, bounds](float value, float progress) {
    setBounds(getX(), getY(), bounds.getWidth() * value, bounds.getHeight() * value);
    float invertedProgress = 1.0f - progress;
    setAlpha(invertedProgress);
  };
  auto completion = [this]() { juce::Component::setVisible(false); };
  EasingAnimator::AnimateInput input = {
      .seconds = 0.06f,
      .completion = completion,
      .animation = animation,
      .range = { 1.00f, 0.95f },
      .points = { 0.50f, 0.75f, 0.30f, 1.20f },
  };
  animator.animate(input);
}

void BasePopup::present() {
  auto p = getPosition();
  setBounds(getX(), getY(), getWidth(), getHeight());
  toFront(false);
  auto bounds = getBounds();
  setAlpha(0.0f);
  setVisible(true);
  auto animation = [this, bounds](float value, float progress) {
    setBounds(getX(), getY(), bounds.getWidth() * value, bounds.getHeight() * value);
    setAlpha(progress);
  };
  auto completion = [this, p, bounds]() {
    setBounds(p.getX(), p.getY(), bounds.getWidth(), bounds.getHeight());
    setAlpha(1.0f);
    this->setInterceptsMouseClicks(true, true);
  };
  EasingAnimator::AnimateInput input = {
    .seconds = 0.06f,
    .completion = completion,
    .animation = animation,
    .range = { 0.95f, 1.00f },
    .points = { 0.50f, 0.75f, 0.30f, 1.20f }
  };
  animator.animate(input);
}
