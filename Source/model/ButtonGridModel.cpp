#include "model/ButtonGridModel.h"

ButtonGridModel::ButtonGridModel(StringArray texts, int column): texts(texts), column(column) { }
void ButtonGridModel::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) { }
int ButtonGridModel::getNumRows() { return texts.size(); }

Component* ButtonGridModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  LabelButton* component;

  if (auto castComponent = static_cast<LabelButton*>(existingComponentToUpdate)) {
    component = castComponent;
  } else {
    component = new LabelButton();
  }

  component->text.setText(texts[rowNumber], dontSendNotification);
  component->on_click_ = [this, rowNumber]() { onClick({ rowNumber, this->column }); };

  return component;
}
