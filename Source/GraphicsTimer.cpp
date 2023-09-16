/*
  ==============================================================================

  GraphicsTimer.cpp
  Created: 8 Sep 2020 12:18:05am
  Author:  dange

  ==============================================================================
*/

#include "GraphicsTimer.h"

GraphicsTimer::~GraphicsTimer() { stopTimer(); }
void GraphicsTimer::stop() { stopTimer(); }

void GraphicsTimer::start() {
  lastUpdateTime = juce::Time::getCurrentTime();
  startTimerHz(fps);
}

int GraphicsTimer::getMillisecondsSinceLastUpdate() const noexcept {
  return (int)(juce::Time::getCurrentTime() - lastUpdateTime).inMilliseconds();
}

GraphicsTimer::GraphicsTimer() {
  lastUpdateTime = juce::Time::getCurrentTime();
}

void GraphicsTimer::timerCallback() {
  float secondsSinceLastUpdate = getMillisecondsSinceLastUpdate() / 1000.0f;
  update(secondsSinceLastUpdate);
  lastUpdateTime = juce::Time::getCurrentTime();
  if (callback) { callback(secondsSinceLastUpdate); }
}

void GraphicsTimer::setFPS(int newFps) {
  this->fps = newFps;
  stop();
  start();
}
