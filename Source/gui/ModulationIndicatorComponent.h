#pragma once

#include "gui/EasingAnimator.h"
#include "gui/PlusComponent.h"
using namespace juce;

class ModulationIndicatorComponent: public juce::Component {
public:
  DrawablePath currentValuePointPath;
  ModulationIndicatorComponent(float magnitude, Colour colour);
  ~ModulationIndicatorComponent() override;
  void resized() override;
  void setMiddleY(float proportion);
  void setMagnitude(float magnitude, bool notifyDelegate = true);
  void setCurrentValue(float value);
  bool isVertical = true;
  void setColour(Colour colour);
  float getPointSize() { return getWidth() - 2.0f; }
  void setBipolar(bool newValue) { this->bipolar = newValue; }
private:
  bool bipolar = false;
  float middleY = 0.0f;
  float magnitude = 0.0f;
  float currentValueY = 0.0f;
  float rawCurrentValue = 0.0f;
  Colour currentPointColour;
  PlusComponent plusComponent;
  Colour trackColour;
  Label label;
  Path trackPath;
  bool isDragging = false;
  inline float getTrackWidth();
  void resizeCurrentValuePoint();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationIndicatorComponent)
};
