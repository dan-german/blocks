/*
  ==============================================================================

    DragIndicatorComponent.cpp
    Created: 29 Oct 2021 2:56:17am
    Author:  Dan German

  ==============================================================================
*/

#include "DragIndicatorComponent.h"

DragIndicatorComponent::DragIndicatorComponent() { setMouseCursor(MouseCursor::DraggingHandCursor); }
DragIndicatorComponent::~DragIndicatorComponent() {}

void DragIndicatorComponent::paint(juce::Graphics& g) {
  g.setColour(colour);
  for (int column = 0; column < columns; column++) {
    float x = column * dotSize + spacing * column;
    for (int row = 0; row < rows; row++) {
      float y = row * dotSize + spacing * row;
      g.fillEllipse(x, y, dotSize, dotSize);
    }
  }
}

void DragIndicatorComponent::resized() { dotSize = getHeight() / rows - spacing * (rows - 2); }
