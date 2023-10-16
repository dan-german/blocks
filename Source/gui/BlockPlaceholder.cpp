/*
  ==============================================================================

    BlockPlaceholder.cpp
    Created: 21 Dec 2021 4:56:52pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/BlockPlaceholder.h"
#include "settings/Constants.h"

BlockPlaceholder::BlockPlaceholder() { start(); }
BlockPlaceholder::~BlockPlaceholder() { }
void BlockPlaceholder::resized() {}
void BlockPlaceholder::setLength(int length) { dotCount = 18.0f * length - ((length - 1) * 6); }

void BlockPlaceholder::paint(juce::Graphics& g) {
  Path path;
  auto bounds = getLocalBounds().toFloat();
  bounds.removeFromBottom(Constants::gridDotSize);
  bounds.removeFromRight(Constants::gridDotSize);
  path.addRoundedRectangle(bounds, 6.5f);

  g.setColour(colour);
  lineLength = path.getLength();
  float addition = lineLength / dotCount;

  for (float i = 0; i < path.getLength(); i += addition) {
    auto index = i + offset;
    if (index >= lineLength) index -= lineLength;

    auto point = path.getPointAlongPath(index);
    auto x = point.getX();
    auto y = point.getY();
    g.fillEllipse(x, y, Constants::gridDotSize, Constants::gridDotSize);
  }
}

void BlockPlaceholder::update(const float secondsSinceLastUpdate) {
  offset += 20.0f * secondsSinceLastUpdate;
  if (offset >= lineLength) {
    offset -= lineLength;
  }
  repaint();
}
