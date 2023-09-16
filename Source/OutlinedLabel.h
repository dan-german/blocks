/*
  ==============================================================================

    OutlinedLabel.h
    Created: 15 Sep 2020 11:14:16pm
    Author:  dange

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;
class OutlinedLabel: public Component {
public:
  String text;
  float trianglEdge = 0;
private:
  void paint(Graphics& g) override;
};
