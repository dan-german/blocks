/*
  ==============================================================================

    ModulatorsSideMenu.h
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "SideMenu.h"
#include "PlusComponent.h"
#include "ModulatorsListModel.h"
#include "Module.h"
#include "ButtonGridPopup.h"

class ModulatorsSideMenu: public SideMenu {
private:
  PlusComponent plusComponent;
  ShapeButton button;
public:
  ModulatorsListModel modulatorsListModel;
  ModulatorsSideMenu();
  void resized() override;
  std::function<void(int)> addedModulator;
  std::function<void(Component&)> onPlusButtonClicked;
  void resizeAddButton();
  void setupAddButton();
  void setupListBox();
  void setup();
};