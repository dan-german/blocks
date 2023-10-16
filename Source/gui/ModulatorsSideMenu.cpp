/*
  ==============================================================================

    ModulatorsSideMenu.cpp
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ModulatorsSideMenu.h"

ModulatorsSideMenu::ModulatorsSideMenu(): button("ModulatorsPlusButton", Colours::transparentBlack, Colours::transparentBlack, Colours::transparentBlack) { setup(); }

void ModulatorsSideMenu::setup() {
  setupAddButton();
  setupListBox();
}

void ModulatorsSideMenu::setupListBox() {
  listBox.setModel(&modulatorsListModel);
  listBox.setRowHeight(106);
}

void ModulatorsSideMenu::setupAddButton() {
  addAndMakeVisible(plusComponent);
  addAndMakeVisible(button);
}

void ModulatorsSideMenu::resized() {
  SideMenu::resized();
  resizeAddButton();
}

void ModulatorsSideMenu::resizeAddButton() {
  int addButtonSize = 12;
  int x = exitButton.getX() + exitButton.getWidth() + 12;
  plusComponent.setAlpha(1.0f);
  plusComponent.plusLineLength = 11;
  plusComponent.setBounds(x, exitButton.getY(), addButtonSize, addButtonSize);
  button.setBounds(plusComponent.getBounds().expanded(4));
}
