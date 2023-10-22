/*
  ==============================================================================

    Utils.h
    Created: 10 Oct 2020 1:49:50am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <span>

class Utils
{
public:
  // Performs linear interpolation using the 2 values wrapped between the given decimal index.
  // e.g. index 0.5 for array [5, 10] returns 7.5.
  static inline float decimalSubscript(std::span<const float> array, float index) {
    int intIndex = (int)(index);
    float fractionalPart = index - intIndex;

    float firstSample = array[intIndex];
    float nextSample = array[intIndex + 1];
    return firstSample + (nextSample - firstSample) * fractionalPart;
  }
};
