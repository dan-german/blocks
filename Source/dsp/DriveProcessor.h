#pragma once

#include "dsp/Processor.h"

inline float soft(float value)
{
#ifdef __linux__
  return tanhf(value);
#else
  return std::tanhf(value);
#endif
}
inline float hard(float value) { return std::clamp(value, -1.0f, 1.0f); }

class DriveProcessor: public Processor {
public:
  DriveProcessor();
  void process(AudioSampleBuffer& inputBuffer) override;
};
