#pragma once

#include "gui/GraphicsTimer.h"
#include "gui/EasingAnimator.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "settings/Constants.h"

using namespace juce;

class BaseButton: public Component {
public:
  GraphicsTimer timer;
  EasingAnimator animator;
  bool orbit = true;
  int reduction_ = 1;

  const float MAX_ORBIT_BRIGHTNESS = 0.1f;

  bool isMouseHovering = false;
  std::function<void()> on_click_;

  Colour colour;
  BaseButton();
protected:
  void mouseEnter(const juce::MouseEvent& event) override;
  void mouseExit(const juce::MouseEvent& event) override;
  void mouseUp(const juce::MouseEvent& event) override;
  virtual void setButtonColour(Colour colour) = 0;

  void paint(juce::Graphics&) override;
  void resized() override;

  virtual void selectedAnimation(float value, float progress);
  virtual void deselectedAnimation(float value, float progress);
  virtual void selectedCompletion();
  virtual void deselectedCompletion();

  float hoverBrightness = 3.0f;
  Point<int> lastMousePosition;
  int mouseDistanceThreshold = 90;
  virtual Component* getContent() = 0;
  virtual void startSelectedAnimation();
  virtual void startDeselectedAnimation();
private:
  void updateOrbitEffect();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseButton)
};