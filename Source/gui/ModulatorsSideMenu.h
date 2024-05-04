/*
  ==============================================================================

    ModulatorsSideMenu.h
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/SideMenu.h"
#include "gui/modulators_list_model.h"
#include "gui/controls/ButtonGridPopup.h"
#include "gui/controls/LabelButton.h"

class ModulatorsSideMenu: public SideMenu {
public:
  ModulatorsListModel modulators_list_model_;
  SVGButton plus_button_;
  ModulatorsSideMenu(BlocksSlider::Listener* listener);
  ~ModulatorsSideMenu();
  std::function<void(int)> on_added_modulator_;

  void resized() override;
  void setupListBox();
};