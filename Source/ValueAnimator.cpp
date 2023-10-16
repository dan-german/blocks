#include "ValueAnimator.h"

ValueAnimator::ValueAnimator() {}
ValueAnimator::~ValueAnimator() {}
inline float ValueAnimator::triangle(float x) { return x < 0.5f ? x : 1 - x; }
inline float ValueAnimator::saw(float x) { return 1.0f - x; }
inline float ValueAnimator::was(float x) { return x; }
void ValueAnimator::reset() { accumulator = 0.0f; }

void ValueAnimator::update(const float deltaTime) {
  if (pause) return;
  value = getWaveform(accumulator);
  accumulator += speed * deltaTime;

  if (accumulator >= 1.0f) {
    accumulator -= 1.0f;

    if (!repeating) {
      stop();
      return;
    }
  }

  if (valueAnimatorCallback != nullptr)
    valueAnimatorCallback(value);
}

float ValueAnimator::getWaveform(float x) {
  switch (waveType) {
  case WaveType::triangle:
    return triangle(x);
  case WaveType::saw:
    return saw(x);
  case WaveType::was:
    return was(x);
  }
  return 0.f;
}
