#pragma once

#include "Processor.h"

inline float soft(float value) { return std::tanhf(value); }
inline float hard(float value) { return std::clamp(value, -1.0f, 1.0f); }

class DriveProcessor: public Processor {
public:
  DriveProcessor();
  void process(AudioSampleBuffer& inputBuffer) override;
};