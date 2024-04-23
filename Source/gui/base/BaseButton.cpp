#include "gui/base/BaseButton.h"
#include "gui/ThemeManager.h"

BaseButton::BaseButton() {
  timer.callback = [this](float) { if (orbit) this->updateOrbitEffect(); };
  timer.start();
  setName("BaseButton");
  setMouseCursor(MouseCursor::PointingHandCursor);
}

void BaseButton::mouseEnter(const juce::MouseEvent& event) {
  isMouseHovering = true;
  startSelectedAnimation();
  // setMouseCursor(MouseCursor::PointingHandCursor);
}

void BaseButton::mouseUp(const juce::MouseEvent& event) {
  if (on_click_) on_click_();
}

void BaseButton::mouseExit(const juce::MouseEvent& event) {
  isMouseHovering = false;
  startDeselectedAnimation();
  // setMouseCursor(MouseCursor::NormalCursor);
}

void BaseButton::resized() {
  getContent()->setBounds(getLocalBounds().reduced(reduction_));
}

void BaseButton::startSelectedAnimation() {
  // printf("startSelectedAnimation\n");
  if (getContent() == nullptr) return;

  EasingAnimator::AnimateInput input;
  input.animation = [this](float value, float progress) { selectedAnimation(value, progress); };
  input.completion = [this]() { selectedCompletion(); };

  float rangeValues[] = { 0.95f, 1.0f };
  std::copy(std::begin(rangeValues), std::end(rangeValues), std::begin(input.range));

  float pointsValues[] = { 0.50f, 0.75f, 0.30f, 1.20f };
  std::copy(std::begin(pointsValues), std::end(pointsValues), std::begin(input.points));

  input.seconds = 0.06f;

  animator.animate(input);
}

void BaseButton::startDeselectedAnimation() {
  EasingAnimator::AnimateInput input;
  input.animation = [this](float value, float progress) { deselectedAnimation(value, progress); };
  input.completion = [this]() { deselectedCompletion(); };

  float rangeValues[] = { 1.00f, 1.20f };
  std::copy(std::begin(rangeValues), std::end(rangeValues), std::begin(input.range));

  input.initialValue = 1.05f;

  float pointsValues[] = { 0.50f, 0.75f, 0.30f, 1.00f };
  std::copy(std::begin(pointsValues), std::end(pointsValues), std::begin(input.points));

  input.seconds = 0.06f;

  animator.animate(input);
}

/* This function brightens the button when the mouse is within a certain distance from it. */
void BaseButton::updateOrbitEffect() {
  auto currentMousePosition = getMouseXYRelative();
  if (currentMousePosition == lastMousePosition || isMouseHovering) return;

  if (currentMousePosition.getDistanceFromOrigin() < mouseDistanceThreshold) {
    auto mapped = jmap((float)currentMousePosition.getDistanceFromOrigin(), 0.0f, 100.0f, MAX_ORBIT_BRIGHTNESS, 0.0f);
    setButtonColour(colour.brighter(mapped));
  } else {
    setButtonColour(colour.brighter(0.0f));
  }

  repaint();
  lastMousePosition = currentMousePosition;
}

void BaseButton::selectedAnimation(float value, float progress) {
  getContent()->setBounds(getLocalBounds().expanded(value));
  setButtonColour(colour.brighter(jmap(progress, 0.0f, 1.0f, 0.0f, hoverBrightness)));
}

void BaseButton::deselectedAnimation(float value, float progress) {
  float brightness = jmap(progress, 0.0f, 1.0f, hoverBrightness, MAX_ORBIT_BRIGHTNESS);
  setButtonColour(colour.brighter(brightness));
}

void BaseButton::selectedCompletion() {
  getContent()->setBounds(getLocalBounds());
  setButtonColour(colour.brighter(hoverBrightness));
}

void BaseButton::deselectedCompletion() {
  getContent()->setBounds(getLocalBounds().reduced(reduction_));
  setButtonColour(colour.brighter());
}

void BaseButton::paint(juce::Graphics& g) {
  colour = ThemeManager::shared()->getCurrent().one;
}