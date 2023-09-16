/*
  ==============================================================================

    EnvelopePath.cpp
    Created: 3 Sep 2021 8:44:55pm
    Author:  Dan German

  ==============================================================================
*/

#include "EnvelopePath.h"
using namespace juce;

EnvelopePath::EnvelopePath() { setInterceptsMouseClicks(false, false); }
EnvelopePath::~EnvelopePath() { }

void EnvelopePath::setAttack(float value) { this->attack = static_cast<float>(std::pow(value, 1.0 / 4));  resized(); repaint(); };
void EnvelopePath::setDecay(float value) { this->decay = static_cast<float>(std::pow(value, 1.0 / 4));   resized(); repaint(); };
void EnvelopePath::setSustain(float value) { this->sustain = value; resized(); repaint(); };
void EnvelopePath::setRelease(float value) { this->release = static_cast<float>(std::pow(value, 1.0 / 4)); resized(); repaint(); };

void EnvelopePath::paint(juce::Graphics& g) {
  Line<float> attack = Line<float>(attackStartX, attackMaxY, attackEndX, attackMinY);
  Line<float> decay = Line<float>(attackEndX, attackMinY, decayEndX, sustainY);
  Line<float> sustain = Line<float>(decayEndX, sustainY, sustainMaxX, sustainY);
  Line<float> release = Line<float>(sustainMaxX, sustainY, releaseEndX, attackMaxY);

  g.setColour(colour);
  g.drawLine(attack, 2.0f);
  g.drawLine(decay, 2.0f);
  g.drawLine(sustain, 2.0f);
  g.drawLine(release, 2.0f);
}

void EnvelopePath::resized() {
  attackStartX = 1;
  attackMaxX = attackStartX + ((getWidth() - attackStartX * 2) / 3.0f);
  attackEndX = juce::jmap(attack, attackStartX, attackMaxX);

  decayMaxX = attackEndX + ((getWidth() - attackStartX * 2) / 3.0f);
  decayEndX = jmap(decay, attackEndX, decayMaxX);

  attackMinY = getHeight() * 0.05f;
  attackMaxY = getHeight() - attackMinY;

  sustainMaxX = attackStartX + ((getWidth() - attackStartX * 2) / 1.5f);
  sustainY = juce::jmap(sustain, attackMaxY, attackMinY);

  releaseEndX = jmap(release, sustainMaxX, getWidth() - attackStartX * 2);
}
