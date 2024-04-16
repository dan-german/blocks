/*
  ==============================================================================

  GraphicsTimer.h
  Created: 8 Sep 2020 12:18:05am
  Author:  dange

  ==============================================================================
*/

#pragma once

#include <juce_events/juce_events.h>

class GraphicsTimer: juce::Timer {
public:
  GraphicsTimer();
  ~GraphicsTimer() override;
  void stop();
  void start();
  std::function<void(float)> callback;
  void setFPS(int fps);
private:
  juce::Time lastUpdateTime;
  void timerCallback() override;
  int getMillisecondsSinceLastUpdate() const noexcept;
  int fps = 60;
protected:
  virtual void update(const float secondsSinceLastUpdate) {};
};
