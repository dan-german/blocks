#include "gui/SideMenu.h"
#include "settings/Constants.h"
#include "gui/ThemeManager.h"

int SideMenu::indexOfModulationConnection(Component* component) { return listBox.getRowNumberOfComponent(component); }
SideMenu::~SideMenu() { }

SideMenu::SideMenu() {
  addAndMakeVisible(exitButton);
  exitButton.onClick = [this]() { setVisible(false); };
  setupList();
}

void SideMenu::setupList() {
  listBox.getVerticalScrollBar().setColour(ScrollBar::ColourIds::thumbColourId, Colour(60, 60, 60));
  listBox.setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
  listBox.setRowHeight(80);
  addAndMakeVisible(listBox);
}

void SideMenu::paint(Graphics& g) {
  auto current = ThemeManager::shared()->getCurrent();
  g.fillAll(current.background);
  g.setColour(current.one.withAlpha(0.5f));

  int lineThickness = 2;
  auto x = isOnLeft ? getWidth() - lineThickness : 0;
  g.fillRect(x, 0, lineThickness, getHeight());
}

void SideMenu::resized() {
  resizeListBox();
  resizeExitButton();
}

void SideMenu::resizeListBox() {
  int headerHeight = 46;
  bounds = getLocalBounds().reduced(borderSize);
  listBox.setBounds(bounds.getX(), headerHeight, bounds.getWidth(), bounds.getHeight() - headerHeight);
}

void SideMenu::resizeExitButton() {
  int buttonSize = 12;
  int insets = 16;
  exitButton.setBounds(insets + borderSize, insets + borderSize, buttonSize, buttonSize);
}