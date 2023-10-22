/*
  ==============================================================================

    BasicWaveforms.h
    Created: 9 May 2021 8:57:43am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include <vector>

struct LookUpTable {
    
  explicit LookUpTable(std::size_t size) : array(size) {}
  LookUpTable(const LookUpTable&) = delete;
  LookUpTable(LookUpTable&&) noexcept = default;
  LookUpTable& operator=(const LookUpTable&) = delete;
  LookUpTable& operator=(LookUpTable&&) noexcept = default;

  std::vector<float> array;
};

class BasicWaveforms {
private:
  static constexpr size_t size = 100;

  inline static LookUpTable sine{size};
  inline static LookUpTable saw{size};
  inline static LookUpTable square{size};
  inline static LookUpTable triangle{size};

  static void setupSine();
  static void setupSaw();
  static void setupSquare();
public:
  BasicWaveforms();

  static float getSine(float x);
  static float getSaw(float x);
  static float getSquare(float x);
  static float getTriangle(float x);
};
