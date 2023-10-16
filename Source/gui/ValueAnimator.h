#pragma once
#include "gui/GraphicsTimer.h"

class ValueAnimator: public GraphicsTimer {
public:
  enum class WaveType { triangle, saw, was };
  WaveType waveType;

  struct Delegate;
  Delegate* delegate;

  ValueAnimator();
  virtual ~ValueAnimator() override;

  void reset();
  float speed = 0.01f;
  bool repeating = true;
  bool pause = false;
  std::function<void(float)> valueAnimatorCallback;
private:
  float getWaveform(float x);
  void update(const float millisecondsSinceLastUpdate) override;
  float value = 0.0f;
  float accumulator = 0.0f;

  float triangle(float x);
  float saw(float x);
  float was(float x);
};

struct ValueAnimator::Delegate {
  virtual void valueAnimatorCallback(float updatedValue) = 0;
  virtual ~Delegate() = default;
};
