#include "DriveProcessor.h"
#include "DriveModule.h"

using Parameters = Model::DriveModule::Parameters;

DriveProcessor::DriveProcessor(): Processor(128) { }

void DriveProcessor::process(AudioSampleBuffer& inputBuffer) {
  Processor::process(inputBuffer);
  auto type = getParameter(Parameters::pType)->getValue(phase);
  float drive = getParameter(Parameters::pDrive)->getValue(phase);

  auto rangedDrive = jmap(drive, 0.0f, 1.0f, 1.0f, 200.0f);
  auto volume = jmap(rangedDrive, 1.0f, 200.0f, 1.0f, 0.2f);

  float (*func)(float) = type == 0 ? soft : hard;

  for (int sample = 0; sample < inputBuffer.getNumSamples(); sample++) {
    inputBuffer.setSample(0, sample, func(inputBuffer.getSample(0, sample) * rangedDrive) * volume);
    inputBuffer.setSample(1, sample, func(inputBuffer.getSample(1, sample) * rangedDrive) * volume);
  }
}
