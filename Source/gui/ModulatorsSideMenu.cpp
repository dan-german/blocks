/*
  ==============================================================================

    ModulatorsSideMenu.cpp
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ModulatorsSideMenu.h"
#include "BinaryData.h"

ModulatorsSideMenu::ModulatorsSideMenu(BlocksSlider::Listener* listener): modulators_list_model_(listener) {
  setupSVGButton(plus_button_, BinaryData::plus_svg, BinaryData::plus_svgSize);
  setupListBox();
}
ModulatorsSideMenu::~ModulatorsSideMenu() { listBox.setModel(nullptr); }

void ModulatorsSideMenu::setupListBox() {
  int row_height = 106;
  listBox.setRowHeight(row_height);
  listBox.setModel(&modulators_list_model_);
}

void ModulatorsSideMenu::resized() {
  SideMenu::resized();
  int x = exit_button_.getRight() + 6;
  int y = exit_button_.getY() - 1;
  int size = exit_button_.getHeight() + 2;
  plus_button_.setBounds(x, y, size, size);
}