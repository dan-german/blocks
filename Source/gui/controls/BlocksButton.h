/*
  ==============================================================================

    BlocksButton.h
    Created: 26 Jun 2021 5:16:57pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class BlocksButton: public Component {
public:
  struct Config {
    Colour textOnColour;
    Colour textOffColour;
  };

  struct Listener;
  enum class Type { fixed, onOff };

  Type type;
  BlocksButton(Colour buttonColour = Colour(35, 35, 35), Type type = Type::onOff);
  ~BlocksButton() override;

  void resized() override;
  std::function<void(BlocksButton*, bool)> onClick;

  Label label;
  virtual void mouseUp(const MouseEvent& event) override;
  Colour textColour;

  Config config;
  void setConfig(Config config);
  void setState(bool state);
private:
  bool selected;
  Path clipAreaPath;
  Path outlinePath;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlocksButton)
};