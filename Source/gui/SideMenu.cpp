#include "gui/SideMenu.h"
#include "settings/Constants.h"
#include "BinaryData.h"
#include "gui/ThemeManager.h"

int SideMenu::indexOfModulationConnection(Component* component) { return listBox.getRowNumberOfComponent(component); }
SideMenu::~SideMenu() { }

SideMenu::SideMenu() {
  exit_button_.on_click_ = [this]() { setVisible(false); };
  setupList();
  setupSVGButton(exit_button_, BinaryData::x_svg, BinaryData::x_svgSize);
}

void SideMenu::setupSVGButton(SVGButton& button, const char* raw_data, size_t size) {
  button.setSVG(raw_data, size);
  addAndMakeVisible(button);
}

void SideMenu::setupList() {
  listBox.getVerticalScrollBar().setColour(ScrollBar::ColourIds::thumbColourId, Colour(60, 60, 60));
  listBox.setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
  listBox.setRowHeight(76);
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
  int buttonSize = 18;
  int insets = 12;
  exit_button_.setBounds(insets + borderSize, insets + borderSize, buttonSize, buttonSize);
}