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
#include "gui/ModulatorsListModel.h"
#include "model/Module.h"
#include "gui/controls/ButtonGridPopup.h"

class ModulatorsSideMenu: public SideMenu {
private:
  PlusComponent plusComponent;
  ShapeButton button;
public:
  ModulatorsListModel modulators_list_model_;
  ModulatorsSideMenu();
  void resized() override;
  std::function<void(int)> on_added_modulator_;
  std::function<void(Component&)> onPlusButtonClicked;
  void resizeAddButton();
  void setupAddButton();
  void setupListBox();
  void setup();
};