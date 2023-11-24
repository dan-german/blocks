#include "MainComponent.h"
#include "ModuleProcessorFactory.h"
#include "model/ModuleFactory.h"
#include "settings/GridConfigs.h"
#include "gui/ThemeManager.h"
#include "settings/UserSettings.h"
#include "module_new.h"

MainComponent::MainComponent(juce::MidiKeyboardState& keyboard_state, Delegate* delegate): delegate(delegate), uiLayer(keyboard_state, this), tabGrid(GridConfigs::tab), blockGrid(GridConfigs::blocks) {
  setWantsKeyboardFocus(false);

  setLookAndFeel(&blocksLookAndFeel);
  setupPopupMenus();
  setupInspector();
  setupUI();
  setupListeners();
  addAndMakeVisible(blockPlaceholder);
  addChildComponent(cursor, 1000);
  setupPresetMenu();
  setupDarkBackground(&gridDarkBackground, 3);
  addChildComponent(uiLayer.modulators);
  setupTabGrid();
  setupBlockGrid();
  setupDarkBackground(&darkBackground, -1);

  noteLogger.listener = this;
  ThemeManager::shared()->set(UserSettings::shared()->getInt("theme", 0));
}

void MainComponent::updateDotPosition(const Point<int> position) {
  cursor.setBounds(position.getX(), position.getY(), cursor.size, cursor.size);
}

void MainComponent::modulatorStartedAdjusting(ModulatorComponent* modulatorComponent, int index) {
  delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, true);
}

void MainComponent::modulatorEndedAdjusting(ModulatorComponent* modulatorComponent, int index) {
  delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, false);
}

void MainComponent::modulatorIsAdjusting(ModulatorComponent* component, int parameter, float value) {
  delegate->editorAdjustedModulator(parameter, component->row, value);
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(ThemeManager::shared()->getCurrent().background);
}

void MainComponent::inspectorGestureChanged(int index, bool started) {
  // std::shared_ptr<Module> focusedModule;

  // auto isTab = tabGrid.containsItem(focusedGridItem);
  // if (isTab) {
  //   focusedModule = delegate->getTab(focusedGridItem->index.column);
  // } else {
  //   focusedModule = delegate->getBlock(focusedGridItem->index);
  // }

  // delegate->editorParameterGestureChanged(focusedModule->name, index, started);
}

void MainComponent::changeModulePainter(int value) {
  auto castedBlock = static_cast<BlockComponent*>(focusedGridItem);
  castedBlock->getPainter()->setWaveformType(static_cast<OscillatorPainter::WaveformType>(value));
}

MainComponent::~MainComponent() {
  setLookAndFeel(nullptr);
}

void MainComponent::setupDarkBackground(DarkBackground* darkBackgroundComponent, int layer) {
  addChildComponent(darkBackgroundComponent, layer);
  darkBackgroundComponent->setOpaque(false);
  darkBackgroundComponent->setInterceptsMouseClicks(true, true);
  darkBackgroundComponent->onClick = [this](Component* c) { darkBackgroundClicked(c); };
}

void MainComponent::darkBackgroundClicked(Component* component) {
  component->setVisible(false);
  Component* popups[] = { &blocksPopup, &modulatorsPopup, &presetsPopup, &savePopup };

  for (auto popup : popups) {
    if (popup->isVisible()) {
      popup->setVisible(false);
    }
  }

  if (focusedGridItem) {
    auto currentGrid = static_cast<GridComponent*>(focusedGridItem->getParentComponent()); // ehh
    toggleGridItemSelection(currentGrid, focusedGridItem, false);
  }

  tabGrid.hideAllItems(false);
  blockGrid.hideAllItems(false);
}

void MainComponent::setupTabGrid() {
  tabGrid.listener = this;
  addAndMakeVisible(tabGrid, 2);

  for (auto glowIndicator : tabGrid.glowIndicators) {
    addChildComponent(glowIndicator);
    glowIndicator->toFront(false);
  }
}

void MainComponent::setupPresetMenu() {
  addChildComponent(presetsPopup);
  presetsPopup.scrollMode = true;
  presetsPopup.itemHeight = 23;
}

void MainComponent::setupBlockGrid() {
  addAndMakeVisible(blockGrid, 4);
  blockGrid.addMouseListener(this, true);
  blockGrid.listener = this;
}

void MainComponent::setupUI() {
  // uiLayer.presetButton.setStrings(delegate->editorRequestsPresetNames());
  uiLayer.modulationsListBoxModel.delegate = this;
  uiLayer.modulators.addMouseListener(this, true);
  addAndMakeVisible(uiLayer, 2);
}

void MainComponent::modulatorStartedDrag(ModulatorComponent* component, const MouseEvent& event) {
  updateDotPosition(event.getEventRelativeTo(this).getPosition());
  enterModulatorDragMode(component->getColour());
}

void MainComponent::setupListeners() {
  timer.start();
  timer.callback = [this](float secondsSinceUpdate) { this->graphicsTimerCallback(secondsSinceUpdate); };

  uiLayer.modulators.modulatorsListModel.modulatorListener = this;

  savePopup.saveButton.onClick = [this]() {
    auto name = savePopup.textEditor.getText();
    bool hasTabs = tabGrid.getItems().size() > 0;
    bool hasModulators = uiLayer.modulators.modulatorsListModel.getNumRows() > 0;
    bool hasBlocks = blockGrid.getItems().size() > 0;

    if (name.isEmpty()) return;
    if (!hasTabs && !hasModulators && !hasBlocks) return;

    delegate->editorSavedPreset(name);
    uiLayer.presetButton.setStrings(delegate->editorRequestsPresetNames());
    savePopup.setVisible(false);
  };

  uiLayer.newPresetButton->onClick = [this]() {
    DBG("changed preset");
    delegate->editorChangedPreset(-1);
    return;
    clear();
  };

  uiLayer.saveButton->onClick = [this]() {
    this->darkBackground.setVisible(true);
    this->darkBackground.toFront(true);
    this->savePopup.setBounds(0, 0, 210, 37);

    int y = this->getHeight() / 12;
    this->savePopup.setCentrePosition(uiLayer.saveButton->getBounds().getCentre().x, y);
    this->savePopup.present();
  };

  uiLayer.modulators.modulatorsListModel.modulatorListener = this;
  uiLayer.modulators.addedModulator = [this](int index) { addModulator(Model::Types::all[index]); };
  uiLayer.presetButton.onClick = [this]() { presetButtonClicked(); };
  uiLayer.themeButton->onClick = [this]() {
    UserSettings::shared()->set("theme", ThemeManager::shared()->next());
    repaint();
  };
}

void MainComponent::addModulator(Model::Type code) {
  auto module = delegate->editorAddedModulator2(code);
  if (module == nullptr) return;
  uiLayer.setModulators(delegate->getModulators2());
}

void MainComponent::resized() {
  uiLayer.setBounds(getLocalBounds());
  resizeGrid();
  resizeInspector();
  resizeTabContainer();
  darkBackground.path.addRectangle(getLocalBounds());
  darkBackground.setBounds(getLocalBounds());
  gridDarkBackground.path.addRectangle(getLocalBounds());
  gridDarkBackground.setBounds(getLocalBounds());

  for (auto glowIndicator : tabGrid.glowIndicators) {
    auto bounds = tabGrid.boundsForItem(glowIndicator, true);
    bounds.removeFromTop(20);
    glowIndicator->setBounds(bounds);
  }
}

void MainComponent::resizeTabContainer() {
  int height = Constants::tabHeight;
  int verticalSpacing = 19;
  tabGrid.setBounds(blockGrid.getX(), blockGrid.getY() - height - verticalSpacing, blockGrid.getWidth(), height);
}

void MainComponent::mouseDown(const MouseEvent& event) {
  if (auto* module = dynamic_cast<GridComponent*>(event.eventComponent))
    return;
}

void MainComponent::mouseUp(const MouseEvent& event) {
  setMouseCursor(MouseCursor::NormalCursor);

  if (darkBackground.isVisible()) {
    darkBackground.setVisible(false);
  }

  if (modulatorDragMode) {
    previousSliderUnderMouse = {};
    modulatorDragMode = false;
    return;
  }

  auto mousePosition = event.getEventRelativeTo(&blockGrid).getPosition();
  auto componentName = event.eventComponent->getName();

  if (componentName == "ModulatorsPlusButton") {
    auto position = event.eventComponent->getPosition() + uiLayer.modulators.getPosition();
    modulatorsPopup.setBounds(position.getX(), position.getY(), 72, 54);
    showPopupAt(modulatorsPopup, [this](Index i) { this->clickOnModulatorsPopup(i); });
  } else if (componentName == "PresetMainButton") {
    auto componentY = event.eventComponent->getPosition().getY();
    auto point = event.eventComponent->getPosition().withY(componentY + 8);
    showPopupAt(presetsPopup, [this](Index i) { this->clickOnModulatorsPopup(i); });
  }

  if (blockGrid.contains(mousePosition)) {
    auto index = blockGrid.indexForPoint(mousePosition); // todo fix 
    clickOnGrid(index);
  }

  cursor.setVisible(false);
}

void MainComponent::clickOnModulatorsPopup(Index index) {
  auto code = index.row == 0 ? Types::lfo : Types::adsr;
  addModulator(code);
  darkBackground.setVisible(false);
}

void MainComponent::clickOnGrid(Index& index) {
  if (blocksPopup.isVisible()) {
    blocksPopup.setVisible(false);
  } else {
    showBlocksPopup(index);
  }
}

void MainComponent::resizeGrid() {
  using namespace Constants;
  const float gridWidth = columns * moduleWidth + (columns - 1) * moduleSpacing + gridEdgeSpacing * 2;
  const float gridHeight = rows * moduleHeight + (rows - 1) * (moduleSpacing)+gridEdgeSpacing * 2;
  const float x = (getLocalBounds().getWidth() - gridWidth) / 2;

  int gridTopOffset = 135;
  blockGrid.setBounds(x, getLocalBounds().getY() + gridTopOffset, gridWidth, gridHeight);

  for (auto moduleComponent : blockGrid.getItems())
    blockGrid.resnapBlock(moduleComponent);
}

void MainComponent::showBlocksPopup(Index index) {
  blockGrid.addButton.setAlpha(0);
  blockGrid.reset();

  auto blockSelectionCompletion = [this, index](Index selectedIndex) {
    int code = selectedIndex.column == 0 ? selectedIndex.row : selectedIndex.row + 5;
    auto module = addBlock(code, index);
    if (module == nullptr) return;
    spawnBlockComponent(module);
  };

  blocksPopup.setBounds(
    blockGrid.addButton.getX() + blockGrid.getX(),
    blockGrid.addButton.getY() + blockGrid.getY(),
    140,
    120
  );

  showPopupAt(blocksPopup, blockSelectionCompletion);
}

void MainComponent::showPopupAt(ButtonGridPopup& popup, std::function<void(Index)> callback) {
  darkBackground.setVisible(true);
  darkBackground.toFront(true);

  auto callbackWrapper = [this, callback, &popup](Index index) {
    darkBackground.setVisible(false);
    popup.setVisible(false);
    popup.setInterceptsMouseClicks(false, false);
    callback(index);
  };

  popup.present(callbackWrapper);
}

std::shared_ptr<model::Module> MainComponent::addBlock(int code, Index index) {
  std::shared_ptr<model::Module> block = nullptr;

  switch (code) { // the first row's 5 codes are reserved for different wave types in the block selection menu popup
  case 0:
  case 1:
  case 2:
  case 3:
  case 4: {
    block = delegate->editorAddedBlock2(Model::Types::osc, index);
    DBG(block->parameters_.size());

    // if (block == nullptr) return nullptr; // todo - grey out the button in the block selection popup if the block is not available
    // auto range = block->parameters[0]->audioParameter->getNormalisableRange();
    // block->parameters[0]->audioParameter->setValue(range.convertTo0to1(code));
    break;
  }
  case 5: block = delegate->editorAddedBlock2(Model::Types::filter, index); break;
    // case 5: block = delegate->editorAddedBlock(Model::Types::filter, index); break;
    // case 6: block = delegate->editorAddedBlock(Model::Types::reverb, index); break;
    // case 7: block = delegate->editorAddedBlock(Model::Types::delay, index); break;
    // case 8: block = delegate->editorAddedBlock(Model::Types::drive, index); break;
    // case 9: block = delegate->editorAddedBlock(Model::Types::mixer, index); break;
  default: break;
  }

  return block;
}

void MainComponent::setupInspector() {
  inspector.addMouseListener(this, true);
  inspector.delegate = this;
  addChildComponent(inspector);
}

void MainComponent::resizeInspector() {
  auto gridY = blockGrid.getY() + blockGrid.getHeight();
  auto gridCenterX = blockGrid.getX() + blockGrid.getWidth() / 2;
  int inspectorX = gridCenterX - inspector.calculateWidth() / 2;
  inspector.setBounds(inspectorX, gridY + 60, 1, 220);
}

void MainComponent::dismissPopup(ButtonGridPopup& popup) {
  popup.setVisible(false);
}

void MainComponent::removeTab(GridItemComponent* tab) {
  if (tab == focusedGridItem) {
    inspector.setVisible(false);
    focusedGridItem = nullptr;
  } else if (inspector.isVisible()) {
    refreshInspector();
  }

  removeChildComponent(tab);

  auto tab_column = tab->index.column;
  tabGrid.detachModule(tab->index, true);

  delegate->editorRemovedTab(tab_column);
  gridDarkBackground.setVisible(false);
}

void MainComponent::removeBlock(GridItemComponent* block) {
  auto item = static_cast<BlockComponent*>(block);
  if (item == focusedGridItem) {
    inspector.setVisible(false);
    focusedGridItem = nullptr;
  } else if (inspector.isVisible()) {
    refreshInspector();
  }
  blocks.removeFirstMatchingValue(item);

  auto index = block->index;
  blockGrid.detachModule(index);
  blockGrid.ResetDotsVisibility();

  delegate->editorRemovedBlock(index);
  uiLayer.setModulations(delegate->getModulations());
  ResetDownFlowingDots();
}

void MainComponent::handleModuleLandedOnInspector(BlockComponent* moduleComponent, const Point<int>& inspectorRelativePosition) {
  blockGrid.resnapBlock(moduleComponent);
}

void MainComponent::inspectorChangedParameter(int sliderIndex, float value) {
  Index moduleIndex(focusedGridItem->index);
  auto isTab = tabGrid.containsItem(focusedGridItem);

  if (isTab) {
    delegate->editorAdjustedTab(moduleIndex.column, sliderIndex, value);
  } else {
    // auto module = delegate->getBlock(moduleIndex);
    delegate->editorAdjustedBlock(moduleIndex, sliderIndex, value);
    // updateModuleComponentVisuals(sliderIndex, value, static_cast<std::shared_ptr<Block>>(module));
  }
}

void MainComponent::updateModuleComponentVisuals(int sliderIndex, float value, std::shared_ptr<Block> block) {
  if (block->id.type == Model::Types::osc) {
    switch (OscillatorModule::Parameters(sliderIndex)) {
    case OscillatorModule::pWave:
      changeModulePainter((int)value);
      break;
    case OscillatorModule::pUnison: {
      auto moduleComponent = blockMatrix[block->index.row][block->index.column];
      if (auto painter = moduleComponent->getPainter())
        painter->setUnison(static_cast<int>(value));
      break;
    }
    default:
      break;
    }
  }
}

void MainComponent::refreshInspector() {
  std::shared_ptr<model::Module> focusedModule;

  // auto isTab = tabGrid.containsItem(focusedGridItem);
  // if (isTab) {
  //   focusedModule = delegate->getTab(focusedGridItem->index.column);
  // } else {
  focusedModule = delegate->getBlock2(focusedGridItem->index);
  // }

  inspector.setConfiguration(focusedModule);
  resizeInspector();
}

PopupMenu MainComponent::spawnModulationMenu(Module& victim) {
  PopupMenu modulateMenu;

  modulateMenu.setLookAndFeel(&blocksLookAndFeel);

  for (int i = 0; i < victim.parameters.size(); i++)
    modulateMenu.addItem(i + 1, victim.parameters[i]->id);

  return modulateMenu;
}

void MainComponent::spawnBlockComponent(std::shared_ptr<model::Module> block) {
  auto blockComponent = BlockComponent::create(block);

  blocks.add(blockComponent);
  blockGrid.addItem(blockComponent, block->index, true);
  blockMatrix[block->index.row][block->index.column] = blockComponent;
  addAndMakeVisible(blockComponent, 1000);
  cursor.setAlwaysOnTop(true);
  if (block->length > 1) blockGrid.setItemLength(blockComponent, block->length);
  // blockComponent->setConfig(block);
  // ResetDownFlowingDots();
}

void MainComponent::spawnTabComponent(std::shared_ptr<Tab> tab) {
  auto tabComponent = TabComponent::create(*tab, &tabGrid);
  addAndMakeVisible(tabComponent, 5);
  tabComponent->toFront(false);
  tabGrid.addItem(tabComponent, { 0, tab->column }, true);
  if (tab->length > 1) tabGrid.setItemLength(tabComponent, tab->length);

  for (auto glowIndicator : tabGrid.glowIndicators)
    glowIndicator->toFront(false);
}

void MainComponent::graphicsTimerCallback(const float secondsSincelastUpdate) {
  return;
  auto currentlyPlayingNotes = delegate->editorRequestsCurrentlyPlayingNotes();
  noteLogger.log(currentlyPlayingNotes);

  if (uiLayer.matrix.isVisible()) {
    auto modulationConnections = delegate->getModulations();

    for (int i = 0; i < modulationConnections.size(); i++) {
      if (auto mc = dynamic_cast<ConnectionComponent*>(uiLayer.matrix.listBox.getComponentForRowNumber(i))) {
        auto value = delegate->editorRequestsModulatorValue(i);
        mc->indicator.setMagnitude(value.second, false);
        mc->indicator.setCurrentValue(value.first);
        mc->indicator.repaint();
      }
    }
  }

  if (focusedGridItem != nullptr) {
    updateInspectorModulationIndicators(); // todo - fix
  }
}

void MainComponent::mouseMove(const MouseEvent& event) {
  this->currentMousePosition = event.getEventRelativeTo(this).getPosition();
}

void MainComponent::updateInspectorModulationIndicators() {
  if (focusedGridItem == nullptr) return;
  if (focusedGridItem->getName() != "BlockComponent") return;

  auto module = delegate->getBlock(focusedGridItem->index);

  for (int parameterIndex = 0; parameterIndex < module->parameters.size(); parameterIndex++) {
    auto parameter = module->parameters[parameterIndex];

    auto range = parameter->audioParameter->getNormalisableRange();
    auto value = parameter->audioParameter->getValue();

    inspector.getSliders()[parameterIndex]->setValue(range.convertFrom0to1(value));

    auto modulators = parameter->connections;

    for (int modulatorIndex = 0; modulatorIndex < parameter->connections.size(); modulatorIndex++) {
      auto modulatorValue = delegate->editorRequestsModulatorValue(focusedGridItem->index, parameterIndex, modulatorIndex);
      inspector.setModulationIndicatorValue(parameterIndex, modulatorIndex, modulatorValue.first, modulatorValue.second);
    }
  }
}

void MainComponent::clear() {
  focusedGridItem = nullptr;
  inspector.setVisible(false);

  for (auto mc : blockGrid.getItems())
    removeChildComponent(mc);

  blockGrid.clear();
  blocks.clear();

  for (auto tab : tabGrid.getItems())
    removeChildComponent(tab);

  tabGrid.clear();

  uiLayer.presetButton.label.setText("empty", dontSendNotification);
  uiLayer.setModulations(delegate->getModulations());
  uiLayer.setModulators(delegate->getModulators2());
  ResetDownFlowingDots();
}

void MainComponent::modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) {
  auto index = uiLayer.matrix.indexOfModulationConnection(component->getParentComponent());
  delegate->editorChangedModulationPolarity(index, bipolar);

  auto connection = delegate->getModulations()[index];

  if (!focusedGridItem) return;
  auto focused = delegate->getBlock(focusedGridItem->index);
  auto target = connection->target;

  if (target->name != focused->name) return;

  auto parameter = target->parameters[connection->parameterIndex];

  for (int m = 0; m < parameter->connections.size(); m++)
    if (connection->source->name == parameter->connections[m]->source->name)
      inspector.getSliders()[connection->parameterIndex]->setModulatorBipolar(m, bipolar);
}

void MainComponent::connectionDeleted(ConnectionComponent* component) {
  auto connection = delegate->getModulations()[component->row];
  if (focusedGridItem) {
    auto focused = delegate->getBlock(focusedGridItem->index);
    auto target = connection->target;

    if (target->name != focused->name) return;

    int index = target->parameters[connection->parameterIndex]->getIndexOfConnection(connection);
    inspector.getSliders()[connection->parameterIndex]->removeIndicator(index);
  }

  delegate->editorDisconnectedModulation(component->row);
  uiLayer.setModulations(delegate->getModulations());

  // if (inspector.isVisible()) inspector.setConfiguration(delegate->getBlock(focusedGridItem->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock(block->index));
}

void MainComponent::sliderValueChanged(Slider* slider) {
  bool isModulatorSlider = slider->getName() == "modulatorSlider";
  if (isModulatorSlider) {
    auto listItemComponent = slider->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent();
    auto index = uiLayer.modulators.listBox.getRowNumberOfComponent(listItemComponent);
    delegate->editorAdjustedModulator(index, 1, static_cast<float>(slider->getValue()));
  } else {
    auto index = uiLayer.matrix.indexOfModulationConnection(slider->getParentComponent()->getParentComponent());
    delegate->editorChangedModulationMagnitude(index, static_cast<float>(slider->getValue()));
  }
}

void MainComponent::loadState(PresetInfo preset) {
  for (auto presetBlock : preset.blocks) {
    auto block = delegate->getBlock(Index { presetBlock.index.first, presetBlock.index.second });
    // spawnBlockComponent(block);
  }

  for (auto presetTab : preset.tabs) {
    auto tab = delegate->getTab(presetTab.column);
    spawnTabComponent(tab);
  }

  uiLayer.setModulations(delegate->getModulations());
  // uiLayer.setModulators(delegate->getModulators());
  uiLayer.presetButton.content.label.setText(preset.name, dontSendNotification);

  for (auto block : blocks) block->animate();
}

void MainComponent::modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) {
  updateDotPosition(event.getEventRelativeTo(this).getPosition());

  if (!inspector.isVisible()) return;

  auto relativePosition = event.getEventRelativeTo(&inspector).getPosition();
  bool draggingInsideInspector = inspector.contains(relativePosition);

  if (draggingInsideInspector) {
    int sliderIndexUnderMouse = (int)ceilf(relativePosition.getX() / inspector.sliderWidth);
    if (sliderIndexUnderMouse == previousSliderUnderMouse) return;

    if (previousSliderUnderMouse.has_value())
      inspector.getSliders()[*previousSliderUnderMouse]->setHighlighted(false);

    previousSliderUnderMouse = sliderIndexUnderMouse;

    auto victim = getFocusedModule();

    if (victim->parameters[sliderIndexUnderMouse]->isModulatable) {
      auto slider = inspector.getSliders()[sliderIndexUnderMouse];
      slider->setHighlighted(true, modulatorComponent->getColour());
    }
  } else {
    if (previousSliderUnderMouse.has_value()) {
      inspector.getSliders()[*previousSliderUnderMouse]->setHighlighted(false);
      previousSliderUnderMouse = {};
    }
  }
}

std::shared_ptr<Module> MainComponent::getFocusedModule() {
  if (focusedGridItem->grid == &blockGrid)
    return delegate->getBlock(focusedGridItem->index);
  else
    return delegate->getTab(focusedGridItem->index.column);
}

void MainComponent::modulatorEndedDrag(ModulatorComponent* modulatorComponent, const MouseEvent& event) {
  exitModulatorDragMode();

  auto gridRelativePosition = event.getEventRelativeTo(&blockGrid).getPosition();
  auto inspectorRelativePosition = event.getEventRelativeTo(&inspector).getPosition();
  auto modulatorIndex = uiLayer.modulators.listBox.getRowNumberOfComponent(modulatorComponent->getParentComponent());

  if (blockGrid.contains(gridRelativePosition)) {
    auto indexUnderMouse = blockGrid.indexForPoint(gridRelativePosition);

    // if landed on a block
    if (auto block = blockGrid.isSlotTaken(indexUnderMouse)) {
      // presentModulationOptionsMenu(modulatorIndex, indexUnderMouse, block); // todo - reimplement
      return;
    }
  } else if (inspector.contains(inspectorRelativePosition)) {
    int parameterIndex = (int)ceilf(inspectorRelativePosition.getX() / inspector.sliderWidth);

    auto focusedModule = getFocusedModule();
    if (focusedModule == nullptr) return;

    auto isModulatable = focusedModule->parameters[parameterIndex]->isModulatable;
    if (!isModulatable) return;

    delegate->editorConnectedModulation(modulatorComponent->row, focusedModule->name, parameterIndex);
    uiLayer.setModulations(delegate->getModulations());
    refreshInspector();

    auto modulator = delegate->getModulator(modulatorIndex);
    auto focusedBlock = blockMatrix[focusedGridItem->index.row][focusedGridItem->index.column];

    focusedBlock->setConfig(focusedModule);
  }
}

void MainComponent::exitModulatorDragMode() {
  setMouseCursor(MouseCursor::NormalCursor);
  darkBackground.setVisible(false);
  cursor.setVisible(false);
}

void MainComponent::presentModulationOptionsMenu(int modulatorIndex, Index& indexUnderMouse, BlockComponent* block) {
  auto blockModel = delegate->getBlock(indexUnderMouse);
  PopupMenu modulateMenu = spawnModulationMenu(*blockModel);

  const int chosenIndex = 1;

  bool discardedMenu = chosenIndex == 0;
  if (discardedMenu) return;

  delegate->editorConnectedModulation(modulatorIndex, blockModel->name, chosenIndex);
  uiLayer.setModulations(delegate->getModulations());

  if (inspector.isVisible()) refreshInspector();
}

void MainComponent::enterModulatorDragMode(Colour colour) {
  darkBackground.setVisible(true);
  cursor.colour = colour;
  cursor.setVisible(true);
  inspector.setAlwaysOnTop(true);
  cursor.setAlwaysOnTop(true);
  modulatorDragMode = true;
}

void MainComponent::modulatorRemoved(ModulatorComponent* component) {
  uiLayer.modulators.modulatorsListModel.remove(component->row);
  uiLayer.modulators.listBox.updateContent();

  delegate->editorRemovedModulator(component->row);
  uiLayer.setModulations(delegate->getModulations());

  // if (inspector.isVisible()) inspector.setConfiguration(delegate->getBlock(focusedGridItem->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock(block->index));
}

void MainComponent::setupPopupMenus() {
  addChildComponent(blocksPopup);
  blocksPopup.setModel({ waveforms, effects });

  addChildComponent(modulatorsPopup);
  modulatorsPopup.setModel(modulators);

  addChildComponent(savePopup);
}

void MainComponent::presetButtonClicked() {
  auto presetButtonBounds = uiLayer.presetButton.getBounds();
  auto y = presetButtonBounds.getY() + presetButtonBounds.getHeight() + 6;

  auto width = 140;
  auto x = presetButtonBounds.getCentreX() - width / 2;

  int presetCount = delegate->editorRequestsPresetNames().size();
  int desiredHeight = presetsPopup.calculateHeight(presetCount);
  int maxHeight = presetsPopup.calculateHeight(10);
  auto height = std::clamp(desiredHeight, desiredHeight, maxHeight);
  presetsPopup.setBounds(x, y, width, height);

  auto presetNames = delegate->editorRequestsPresetNames();
  presetsPopup.setModel({ presetNames });

  showPopupAt(presetsPopup, [this](Index i) { this->loadPreset(i.row); });
}

void MainComponent::loadPreset(int index) {
  clear();
  auto preset = delegate->editorChangedPreset(index);
  loadState(preset);
  darkBackground.setVisible(false);
}

void MainComponent::visibilityChanged() {
  // loadState(delegate->getStateRepresentation());
}

void MainComponent::clickedOnGrid(GridComponent* grid, Index index) {
  if (grid == &blockGrid) return;
  spawnTabComponent(delegate->editorAddedTab(index.column));
}

void MainComponent::gridItemRemoved(GridComponent* grid, GridItemComponent* item) {
  if (grid == &blockGrid) {
    removeBlock(item);
  }
}

void MainComponent::gridItemRepositioned(GridComponent* grid, GridItemComponent* item, Index oldIndex) {
  if (grid == &blockGrid) {
    blockMatrix[oldIndex.row][oldIndex.column] = nullptr;
    blockMatrix[item->index.row][item->index.column] = static_cast<BlockComponent*>(item);
    delegate->editorRepositionedBlock(oldIndex, item->index);
    ResetDownFlowingDots();
  } else if (grid == &tabGrid) {
    delegate->editorRepositionedTab(oldIndex.column, item->index.column);
  }
}

void MainComponent::gridItemLengthChanged(GridComponent* grid, GridItemComponent* item, int length) {
  if (grid == &blockGrid) {
    delegate->editorChangedBlockLength(item->index, length);
  } else if (grid == &tabGrid) {
    delegate->editorChangedTabLength(item->index.column, length);
  }
}

void MainComponent::gridItemHovered(GridComponent* grid, GridItemComponent* item, Index index) {
  if (grid == &tabGrid) {
    blockGrid.highlightColumn(index.column, index.column + item->length);
  }
}

void MainComponent::gridItemStartedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tabGrid) {
    tabGrid.hideAllItems(true, item);
    gridDarkBackground.setVisible(true);
  }
}

void MainComponent::gridItemEndedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tabGrid) {
    gridDarkBackground.setVisible(false);
    tabGrid.hideAllItems(false, item);
    blockGrid.reset();
    blockGrid.ResetDotsVisibility();
  }
}

void MainComponent::gridItemClicked(GridComponent* grid, GridItemComponent* item, const MouseEvent& event) {
  if (event.mods.isRightButtonDown() && grid == &tabGrid) {
    removeTab(item);
    return;
  }

  toggleGridItemSelection(grid, item, !item->isSelected);
}

void MainComponent::toggleGridItemSelection(GridComponent* grid, GridItemComponent* item, bool selected) {
  gridDarkBackground.setVisible(selected);
  inspector.setVisible(selected);

  if (selected) {
    item->setHidden(false);
    bool otherModuleCurrentlyChosen = focusedGridItem != nullptr;
    if (otherModuleCurrentlyChosen) focusedGridItem->setSelected(false);
    focusedGridItem = item;
    refreshInspector();
  } else {
    focusedGridItem = nullptr;
  }

  item->setSelected(selected);
  blockGrid.hideAllItems(selected, item);
  tabGrid.hideAllItems(selected, item);
}

void MainComponent::notesStarted(Array<int> notes) {
  tabGrid.animateGlowIndicators(delegate->editorRequestsActiveColumns());
}

void MainComponent::notesEnded(Array<int> notes) { }

void MainComponent::ResetDownFlowingDots() {
  std::set<int> columns_with_blocks;
  for (auto block_component : blocks) {
    auto block_model = delegate->getBlock(block_component->index);
    if (block_model->id.type != Model::Types::osc) continue;
    columns_with_blocks.insert(block_component->index.column);
  }

  for (int column = 0; column < GridConfigs::blocks.columns; column++) {
    blockGrid.SetDownFlowingHighlight(column, false);
  }

  for (auto column : columns_with_blocks) {
    blockGrid.SetDownFlowingHighlight(column, true);
  }
}
