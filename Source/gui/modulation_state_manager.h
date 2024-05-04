#include "connection.h"
#include "controls/blocks_slider.h"
#include "ModulatorComponent.h"

namespace gui {
class ModulationStateManager {
public:
  class Listener;
  ModulationStateManager(Listener* listener, Component& base_component);
  void deactivate();
  void activate(const std::vector<std::shared_ptr<model::Connection>>& connections);
  void process(ModulatorComponent* modulator_component, const juce::MouseEvent& event);
private:
  Listener* listener_;
  std::vector<std::shared_ptr<model::Connection>> current_connections_;
  Component* last_hovered_slider_ = nullptr;
  Component& base_component_;
  bool is_currently_modulating_slider_ = false;

  void handleNoComponentFound(const ModulatorComponent* modulator_component);
  void handleModulationHoverEnd(const ModulatorComponent* modulator_component);
  void handleModulationHover(const ModulatorComponent* modulator_component);
};

struct ModulationStateManager::Listener {
  virtual void hovered(BlocksSlider* blocks_slider, const ModulatorComponent* modulator_component) = 0;
  virtual void unhovered(BlocksSlider* blocks_slider, const ModulatorComponent* modulator_component) = 0;
};
}