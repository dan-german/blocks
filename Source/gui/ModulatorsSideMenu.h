/*
  ==============================================================================

    ModulatorsSideMenu.h
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/SideMenu.h"
#include "gui/PlusComponent.h"
#include "gui/modulators_list_model.h"
// #include "model/Module.h"
#include "gui/controls/ButtonGridPopup.h"
#include "gui/controls/LabelButton.h"

class ModulatorsSideMenu: public SideMenu {
private:
  ShapeButton button;
  void mouseUp(const MouseEvent& event) override;
public:
  ModulatorsListModel modulators_list_model_;
  PlusComponent plus_button;
  ModulatorsSideMenu(BlocksSlider::Listener* listener);
  ~ModulatorsSideMenu();
  std::function<void(int)> on_added_modulator_;
  std::function<void(const juce::MouseEvent& event)> plus_button_callback;

  void resized() override;
  void resizeAddButton();
  void setupAddButton();
  void setupListBox();
  void setup();
};