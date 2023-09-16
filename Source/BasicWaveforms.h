/*
  ==============================================================================

    BasicWaveforms.h
    Created: 9 May 2021 8:57:43am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

template <int table_size>
class LookUpTable {
public:
  int size = table_size;
  float* array = new float[table_size];
};

class BasicWaveforms {
private:
  static const int size = 100;

  inline static LookUpTable<size> sine;
  inline static LookUpTable<size> saw;
  inline static LookUpTable<size> square;
  inline static LookUpTable<size> triangle;

  static void setupSine();
  static void setupSaw();
  static void setupSquare();
public:
  BasicWaveforms();
  ~BasicWaveforms();

  static float getSine(float x);
  static float getSaw(float x);
  static float getSquare(float x);
  static float getTriangle(float x);
};