#include "gui/controls/ButtonGrid.h"

#include "gui/ThemeManager.h"

ButtonGrid::ButtonGrid() {}

int ButtonGrid::getNumberOfColumns() { return numberOfColumns; }
int ButtonGrid::getNumberOfRows() { return numberOfRows; }

void ButtonGrid::present(std::function<void(Index)> callback) {
  getParentComponent()->setVisible(true);
  this->onClick = callback;
}

void ButtonGrid::resized() {
  for (int i = 0; i < listBoxes.size(); i++) {
    auto* listBox = listBoxes[i].get();
    listBox->setRowHeight(itemHeight);
    auto localBounds = getLocalBounds();
    int width = getWidth() / listBoxes.size();

    int x = i * width;
    int y = localBounds.getY();
    int height = getHeight();

    listBox->setBounds(x, y, width, height);
    listBox->setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
  }
}

void ButtonGrid::setModel(Array<StringArray> models) {
  listBoxModels.clear();
  listBoxes.clear();

  for (int i = 0; i < models.size(); i++) {
    auto model = models.getUnchecked(i);
    auto listBoxModel = std::make_unique<ButtonGridModel>(model, i);
    listBoxModel->onClick = [this](Index index) { this->onClick(index); };

    auto listBox = std::make_unique<ListBox>();
    listBox->getVerticalScrollBar().setColour(ScrollBar::ColourIds::thumbColourId, Colour(60, 60, 60));
    listBox->setModel(listBoxModel.get());


    addAndMakeVisible(listBox.get());

    listBox->setRowHeight(itemHeight);
    listBox->setColour(ListBox::ColourIds::backgroundColourId, ThemeManager::shared()->getCurrent().background);
    
    listBoxModels.push_back(std::move(listBoxModel));
    listBoxes.push_back(std::move(listBox));

    numberOfRows = std::max(numberOfRows, model.size());
  }

  numberOfColumns = models.size();
  resized();
}