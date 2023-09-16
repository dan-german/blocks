#include "ModulationIndicatorComponent.h"

ModulationIndicatorComponent::ModulationIndicatorComponent(float initalMagnitude,
  Colour colour): magnitude(initalMagnitude),
  currentPointColour(colour) {
  trackColour = colour.withAlpha(0.0f);

  currentPointColour = colour;
  currentValuePointPath.setFill(FillType(currentPointColour));
  addAndMakeVisible(currentValuePointPath);

  setMagnitude(magnitude, false);

  bipolar = false;
}

ModulationIndicatorComponent::~ModulationIndicatorComponent() {}

void ModulationIndicatorComponent::resized() { resizeCurrentValuePoint(); }
void ModulationIndicatorComponent::setMiddleY(float proportion) { middleY = proportion; }
void ModulationIndicatorComponent::setMagnitude(float magnitude, bool notifyDelegate) { this->magnitude = magnitude; }
float ModulationIndicatorComponent::getTrackWidth() { return getWidth() * 0.70f; }

void ModulationIndicatorComponent::resizeCurrentValuePoint() { // this should be refactored
  if (isVertical) { // isVertical is true for inspector sliders, false for matrix...
    float pointSize = getWidth() - 2.0f;
    float midY = middleY * (getHeight() - pointSize);
    float x = getWidth() / 2.0f - pointSize / 2;
    float y;
    float clampedY;

    if (bipolar) {
      y = (currentValueY - 0.5f) * (getHeight() - pointSize) * magnitude;
      clampedY = std::clamp(midY - y, 0.0f, getHeight() - pointSize);
    } else {
      y = currentValueY * (getHeight() - pointSize) * magnitude;
      clampedY = std::clamp(midY - y, 0.0f, getHeight() - pointSize);
    }

    Path p;
    p.addRoundedRectangle(x, clampedY, pointSize, pointSize, pointSize / 2);

    currentValuePointPath.setPath(p);
  } else {
    float pointSize = getHeight() - 2.0f;
    float y = getHeight() / 2.0f - pointSize / 2.0f;
    float addition = (getWidth() / 2 - pointSize) * currentValueY * magnitude;
    float x;

    if (bipolar)
      x = (getWidth() / 2) + (rawCurrentValue * magnitude * (getWidth() / 4.0f));
    else
      x = (getWidth() / 2 - pointSize) + (getHeight() - pointSize) + addition;

    Path p;
    p.addRoundedRectangle(x, y, pointSize, pointSize, pointSize / 2);

    currentValuePointPath.setPath(p);
  }
}

void ModulationIndicatorComponent::setCurrentValue(float value) {
  if (std::isnan(value)) value = 0.0f;

  currentValueY = jmap(value, -1.0f, 1.0f, 0.0f, 1.0f);
  rawCurrentValue = value;

  resizeCurrentValuePoint();
}

void ModulationIndicatorComponent::setColour(Colour colour) {
  currentPointColour = colour;
  currentValuePointPath.setFill(FillType(currentPointColour));
}
