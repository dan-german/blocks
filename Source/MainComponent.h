#pragma once

#include "InspectorComponent.h"
#include "Module.h"
#include "BlocksLookAndFeel.h"
#include "Modulation.h"
#include "ColourPool.h"
#include "UILayer.h"
#include "PresetInfo.h"
#include "GraphicsTimer.h"
#include "ModulatorComponent.h"
#include "Cursor.h"
#include "BlockPlaceholder.h"
#include "ModelConstants.h"
#include "TabContainerComponent.h"
#include "TabComponent.h"
#include "BlockGridComponent.h"
#include "Tab.h"
#include "NoteLogger.h"

using Modulation = Model::Modulation;
using Block = Model::Block;
using Tab = Model::Tab;

class MainComponent final: public Component,
  InspectorComponent::Listener,
  Slider::Listener,
  ConnectionComponent::Listener,
  ModulatorComponent::Listener,
  GridComponent::Listener,
  NoteLogger::Listener {
public:
  struct Delegate;
  Delegate* delegate;
  MainComponent(Delegate* delegate);
  ~MainComponent() override;
  void mouseMove(const MouseEvent& event) override;

  BlocksLookAndFeel blocksLookAndFeel;
  UILayer uiLayer;
  void loadState(PresetInfo preset);
  void visibilityChanged() override;
protected:
  void paint(juce::Graphics&) override;
  void resized() override;

  // MouseListener
  void mouseDown(const MouseEvent& event) override;
  void mouseUp(const MouseEvent& event) override;
private:
  DarkBackground darkBackground;
  DarkBackground gridDarkBackground;
  BlockGridComponent blockGrid;
  TabContainerComponent tabGrid;
  InspectorComponent inspector;
  int presetIndex = 0;
  StringArray presetNames;
  SavePopup savePopup;
  GraphicsTimer timer;

  Array<BlockComponent*> blocks;
  GridItemComponent* focusedGridItem = nullptr;
  Point<int> currentMousePosition;

  BlockComponent* blockMatrix[Constants::rows][Constants::columns];
  Cursor cursor;
  std::optional<int> previousSliderUnderMouse = {};
  BlockPlaceholder blockPlaceholder;
  bool isBlocksMenuVisible = false;
  bool modulatorDragMode = false;
  ButtonGridPopup blocksPopup;
  ButtonGridPopup modulatorsPopup;
  ButtonGridPopup presetsPopup;
  NoteLogger noteLogger;

  void setupInspector();
  void clear();
  void resizeGrid();
  void resizeTabContainer();
  void resizeInspector();
  void clickOnModulatorsPopup(Index index);
  void loadPreset(int index);
  void setupPopupMenus();
  void darkBackgroundClicked(Component* darkBackground);
  void setupDarkBackground(DarkBackground* component, int layer);

  void toggleGridItemSelection(GridComponent* grid, GridItemComponent* item, bool selected);
  void showBlocksPopup(Index index);
  std::shared_ptr<Block> addBlock(int code, Index index);
  void removeBlock(GridItemComponent* block);
  void removeTab(GridItemComponent* tab);
  std::shared_ptr<Module> getFocusedModule();

  // Grid Listener
  void clickedOnGrid(GridComponent* grid, Index index) override;
  void gridItemRemoved(GridComponent* grid, GridItemComponent* item) override;
  void gridItemRepositioned(GridComponent* grid, GridItemComponent* item, Index oldIndex) override;
  void gridItemLengthChanged(GridComponent* grid, GridItemComponent* item, int length) override;
  void gridItemHovered(GridComponent* grid, GridItemComponent* item, Index index) override;
  void gridItemClicked(GridComponent* grid, GridItemComponent* item, const MouseEvent& event) override;
  void gridItemStartedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) override;
  void gridItemEndedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) override;

  void modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) override;
  // NoteLogger::Listener
  void notesStarted(Array<int> notes) override;
  void notesEnded(Array<int> notes) override;

  // InspectorComponent::Listener
  void inspectorChangedParameter(int sliderIndex, float value) override;
  void inspectorGestureChanged(int index, bool started) override;

  void dismissPopup(ButtonGridPopup& popup);
  void spawnBlockComponent(std::shared_ptr<Block> block);
  void spawnTabComponent(std::shared_ptr<Tab> tab);
  void graphicsTimerCallback(const float secondsSinceLastUpdate);
  void changeModulePainter(int value);
  PopupMenu spawnModulationMenu(Module& victim);
  void showPopupAt(ButtonGridPopup& popupWrapper, std::function<void(Index)> callback);
  void updateInspectorModulationIndicators();
  void handleModuleLandedOnInspector(BlockComponent* moduleComponent, const Point<int>& inspectorRelativePosition);
  void refreshInspector();
  void connectModulation(BlockComponent* moduleComponent, Index& victimIndex, int parameterIndex);
  void setupUI();
  void sliderValueChanged(Slider* slider) override;
  void setupBlockGrid();
  void setupTabGrid();
  void updateModuleComponentVisuals(int sliderIndex, float value, std::shared_ptr<Block> block);
  void setupListeners();
  void clickOnGrid(Index& index);

  void modulatorEndedDrag(ModulatorComponent* modulatorComponent, const MouseEvent& event) override;
  void modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) override;
  void modulatorStartedDrag(ModulatorComponent* component, const MouseEvent& event) override;
  void modulatorStartedAdjusting(ModulatorComponent* modulatorComponent, int index) override;
  void modulatorEndedAdjusting(ModulatorComponent* modulatorComponent, int index) override;
  void modulatorIsAdjusting(ModulatorComponent* component, int parameter, float value) override;

  void presentModulationOptionsMenu(int modulatorIndex, Index& indexUnderMouse, BlockComponent* block);
  void updateDotPosition(const Point<int> position);
  void addModulator(Model::Type code);
  void enterModulatorDragMode(Colour colour);
  void connectionDeleted(ConnectionComponent* component) override;

  void exitModulatorDragMode();
  void modulatorRemoved(ModulatorComponent* component) override;
  void setupPresetMenu();
  void presetButtonClicked();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

struct MainComponent::Delegate {
  virtual std::shared_ptr<Tab> editorAddedTab(int column) = 0;
  virtual void editorRepositionedTab(int oldColumn, int newColumn) = 0;
  virtual void editorChangedTabLength(int column, int times) = 0;
  virtual void editorRemovedTab(int column) = 0;

  virtual void editorRepositionedBlock(Index oldIndex, Index newIndex) = 0;
  virtual void editorRemovedBlock(Index index) = 0;
  virtual void editorAdjustedBlock(Index moduleIndex, int parameterIndex, float value) = 0;
  virtual void editorAdjustedTab(int column, int parameterIndex, float value) = 0;
  virtual void editorAdjustedModulator(int modulatorIndex, int parameterIndex, float value) = 0;
  virtual void editorConnectedModulation(int modulatorIndex, String targetName, int parameterIndex) = 0;

  virtual void editorChangedModulationMagnitude(int modulationConnectionIndex, float magnitude) = 0;
  virtual void editorChangedModulationPolarity(int index, bool bipolar) = 0;
  virtual void editorDisconnectedModulation(int index) = 0;
  virtual void editorParameterGestureChanged(String moduleName, int parameterIndex, bool started) = 0;
  virtual void editorRemovedModulator(int index) = 0;
  virtual void editorChangedBlockLength(Index index, int times) = 0;
  virtual void editorSavedPreset(String name) = 0;

  virtual Array<MPENote> editorRequestsCurrentlyPlayingNotes() = 0;
  virtual StringArray editorRequestsPresetNames() = 0;
  virtual PresetInfo editorChangedPreset(int index) = 0;
  virtual PresetInfo getStateRepresentation() = 0;

  virtual std::pair<float, float> editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) = 0;
  virtual std::pair<float, float> editorRequestsModulatorValue(int modulationConnectionIndex) = 0;
  virtual std::shared_ptr<Tab> getTab(int column) = 0;
  virtual std::shared_ptr<Module> getModulator(int index) = 0;
  virtual std::shared_ptr<Module> editorAddedModulator(Model::Type code) = 0;
  virtual std::shared_ptr<Block> getBlock(Index index) = 0;
  virtual std::shared_ptr<Block> editorAddedBlock(Model::Type code, Index index) = 0;
  virtual Array<int> editorRequestsActiveColumns() = 0;
  virtual Array<std::shared_ptr<Module>> getModulators() = 0;
  virtual Array<std::shared_ptr<Modulation>> getModulations() = 0;
  virtual Array<std::shared_ptr<Modulation>> getConnectionsOfSource(std::shared_ptr<Module> source) = 0;

  virtual ~Delegate() = default;
};