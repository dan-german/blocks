/*
  ==============================================================================

    ModulatorsSideMenu.cpp
    Created: 30 Oct 2021 3:55:41pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ModulatorsSideMenu.h"

ModulatorsSideMenu::ModulatorsSideMenu(): button("ModulatorsPlusButton", Colours::transparentBlack, Colours::transparentBlack, Colours::transparentBlack) { setup(); }
ModulatorsSideMenu::~ModulatorsSideMenu() { listBox.setModel(nullptr); }

void ModulatorsSideMenu::setup() {
  setupAddButton();
  setupListBox();
  // plus_button.addMouseListener(this, false);
  // addMouseListener(&button, false);
  button.addMouseListener(this, false); 
}

void ModulatorsSideMenu::setupListBox() {
  listBox.setModel(&modulators_list_model_);
  listBox.setRowHeight(106);
}

void ModulatorsSideMenu::setupAddButton() {
  addAndMakeVisible(plus_button);
  addAndMakeVisible(button);
  // addAndMakeVisible(plus_button2);
}

void ModulatorsSideMenu::resized() {
  SideMenu::resized();
  resizeAddButton();
}

void ModulatorsSideMenu::mouseUp(const MouseEvent& event) {
  auto relative_position = event.getEventRelativeTo(this).getPosition();
  if (button.getBounds().contains(relative_position)) {
    if (plus_button_callback) {
      plus_button_callback(event);
    }
  }
}

void ModulatorsSideMenu::resizeAddButton() {
  int addButtonSize = 12;
  int x = exitButton.getX() + exitButton.getWidth() + 12;
  plus_button.setAlpha(1.0f);
  plus_button.plusLineLength = 11;
  plus_button.setBounds(x, exitButton.getY(), addButtonSize, addButtonSize);
  button.setBounds(plus_button.getBounds().expanded(4));
}
