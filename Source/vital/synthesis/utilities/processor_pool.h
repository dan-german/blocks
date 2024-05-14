#include "vital/synthesis/framework/synth_module.h"
#include "model/ModelConstants.h"
#include "vital/common/line_generator.h"
#include "vital/synthesis/modules/oscillator_module.h"

class ProcessorPool {
public:
  std::vector<vital::OscillatorModule*> oscillators_;
  class SetupInput;
  void spawn(SetupInput& input);
  vital::SynthModule* fetch(std::string& code);
  void spawnProcessors(std::string code, SetupInput& input);
private:
  std::unordered_map<std::string, std::vector<vital::SynthModule*>> map_;
  // LineGenerator lfo_sources_[model::MAX_MODULES_PER_TYPE];
  vital::SynthModule* processorFromCode(std::string code, SetupInput& input);
};

struct ProcessorPool::SetupInput {
  vital::SynthModule* parent;
  vital::Output* reset;
  vital::Output* active_mask;
  vital::Output* retrigger;
  vital::Processor* bent_midi;
  vital::Output* beats_per_second;
  vital::Output* note_count;
  vital::Output* note_from_ref;
};