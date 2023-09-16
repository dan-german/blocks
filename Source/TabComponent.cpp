#include "TabComponent.h"
#include "TabContainerComponent.h"

TabComponent::~TabComponent() {
  ThemeManager::shared()->removeListener(this);
}

TabComponent::TabComponent(int column, int width, GridComponent* grid): BaseTabComponent(column, width, grid) {
  auto theme = ThemeManager::shared()->getCurrent();
  label.setColour(Label::ColourIds::textColourId, theme.background);
  label.setText("A", NotificationType::dontSendNotification);
  label.setFont(Font(12, Font::bold));
  label.setInterceptsMouseClicks(false, false);
  label.setBorderSize(BorderSize<int>(0));
  label.setJustificationType(Justification::centred);
  addAndMakeVisible(label);
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

void TabComponent::resized() {
  auto width = getWidth();
  auto height = getHeight();
  auto x = getWidth() / 2 - width / 2;
  auto y = getHeight() / 2 - height / 2;
  label.setBounds(x, y, width, height);
}

void TabComponent::paint(juce::Graphics& g) {
  BaseTabComponent::paint(g);
  if (stretchable)
    drawStretchIndicator(g);
}

void TabComponent::drawStretchIndicator(juce::Graphics& g) {
  int dotSize = 2;
  int spacing = 2;
  int count = 3;
  int height = dotSize * count + spacing * count;
  int startY = getHeight() / 2 - height / 2 + 1;
  for (int i = 0; i < 3; i++) {
    int y = startY + i * dotSize + i * spacing;
    auto theme = ThemeManager::shared()->getCurrent();
    g.setColour(theme.background);
    g.fillEllipse(getWidth() - 8, y, dotSize, dotSize);
  }
}

TabComponent* TabComponent::create(Tab& tab, TabContainerComponent* container) {
  const auto tabComponent = new TabComponent(tab.column, container->getConfig().itemWidth, container);
  tabComponent->label.setText(tab.name, juce::NotificationType::dontSendNotification);
  tabComponent->listener = container;
  tabComponent->length = tab.length;
  return tabComponent;
}

void TabComponent::setHidden(bool hidden) {
  this->isHidden = hidden;
  auto current = ThemeManager::shared()->getCurrent();
  bgColour = hidden ? Colours::transparentBlack : ThemeManager::shared()->getCurrent().two;
  label.setColour(Label::ColourIds::textColourId, hidden ? current.one : current.background);
  repaint();
}

void TabComponent::setSelected(bool selected) {
  GridItemComponent::setSelected(selected);
  auto theme = ThemeManager::shared()->getCurrent();
  bgColour = selected ? theme.three : theme.two;
  auto labelSelectedColour = theme.dark ? theme.one : theme.two;
  label.setColour(Label::ColourIds::textColourId, selected ? labelSelectedColour : theme.background);
}

void TabComponent::themeChanged(Theme theme) {
  BaseTabComponent::themeChanged(theme);
  label.setColour(Label::ColourIds::textColourId, theme.background);
}