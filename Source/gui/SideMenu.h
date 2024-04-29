/*
  ==============================================================================

    SideMenu.h
    Created: 11 Jun 2021 1:47:05pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/controls/ExitButton.h"
#include "gui/controls/SVGButton.h"
#include "model/connection_list_model.h"
// #include "model/Modulation.h"


class SideMenu: public Component {
public:
  SideMenu();
  ~SideMenu() override;
  void resized() override;
  void paint(Graphics& g) override;
  ExitButton exitButton;
  SVGButton exit_button_;
  ListBox listBox;
  int indexOfModulationConnection(Component* component);
  bool isOnLeft = true;

  void setupSVGButton(SVGButton& button, const char* rawData, size_t size);
private:
  int borderSize = 8;
  Label title;
  void setupList();
  void resizeExitButton();
  Rectangle<int> bounds;

  void resizeListBox();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SideMenu)
};
