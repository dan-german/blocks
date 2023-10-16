/*
  ==============================================================================

    OscillatorPainter.cpp
    Created: 8 Nov 2020 1:39:46am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/OscillatorPainter.h"
#include "util/Utils.h"

#define MAX_PAINTER_UNISON 4

OscillatorPainter::OscillatorPainter(): waveColour(Colours::white.withAlpha(0.15f)) {
  setInterceptsMouseClicks(false, false);
  start();

  for (int i = 0; i < 4; i++)
    paths[i] = Path();
}

OscillatorPainter::~OscillatorPainter() { }

void OscillatorPainter::resized() {}

void OscillatorPainter::paint(juce::Graphics& g) {
  int componentCenterY = getHeight() / 2;

  if (waveformType == WaveformType::noise) {
    for (float x = 0.0f; x < getWidth(); x += resolution) {
      float y = getWaveform(x + offset);
      float halfHeight = getHeight() / 2.0f - thickness;
      float mappedY = jmap(y, -1.0f, 1.0f, componentCenterY - halfHeight, componentCenterY + halfHeight);

      auto point = Point<float>(x, mappedY);

      if (x == 0.0f) {
        paths[0].startNewSubPath(point);
      } else {
        paths[0].lineTo(point);
      }
    }
  } else {
    for (int i = 0; i < unison; i++) {
      float xOffset = i * 3.0f;
      float yOffset = i * 1.5f;

      for (float x = xOffset; x < getWidth(); x += resolution) {
        float y = getWaveform(x + offset);
        float halfHeight = getHeight() / 2.0f - thickness;
        float mappedY = jmap(y, -1.0f, 1.0f, componentCenterY - halfHeight, componentCenterY + halfHeight);

        auto point = Point<float>(x - xOffset, mappedY + yOffset);

        if (x == xOffset) {
          paths[i].startNewSubPath(point);
        } else {
          paths[i].lineTo(point);
        }
      }
    }
  }

  for (int i = unison - 1; i >= 0; i--) {
    float alpha = std::lerp(1.0f, 0.01f, i / float(unison));

    g.setColour(waveColour.withAlpha(alpha));
    g.strokePath(paths[i], PathStrokeType(thickness));
    paths[i].clear();
  }
}

void OscillatorPainter::update(const float secondsSinceLastUpdate) {
  offset += speed * secondsSinceLastUpdate;
  repaint();
}

void OscillatorPainter::setUnison(int unison) {
  if (waveformType == WaveformType::noise) return;
  this->unison = std::clamp(unison, 1, MAX_PAINTER_UNISON);
}

void OscillatorPainter::setWaveformType(OscillatorPainter::WaveformType waveformType) {
  if (this->waveformType == waveformType) return;
  this->waveformType = waveformType;
  setFPS(waveformType == WaveformType::noise ? 15 : 60);
  repaint();
}