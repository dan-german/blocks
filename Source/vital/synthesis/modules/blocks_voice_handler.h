/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/modules/producers_module.h"
#include "vital/synthesis/framework/voice_handler.h"
#include "vital/synthesis/lookups/wavetable.h"
#include "vital/common/synth_types.h"
#include "vital/common/line_generator.h"
#include "vital/synthesis/modules/filters_module.h"
#include "vital/synthesis/modules/chorus_module.h"
#include "vital/synthesis/modules/compressor_module.h"
#include "vital/synthesis/modules/phaser_module.h"
#include "model/Index.h"
#include <vector>
#include "ModuleContainer.h"
 // #include "osc.h"
#include "vital/synthesis/modules/oscillator_module.h"
#include "model/module_manager.h"

namespace vital {
class AudioRateEnvelope;
class FiltersModule;
class LegatoFilter;
class LineMap;
class LfoModule;
class EnvelopeModule;
class RandomLfoModule;
class TriggerRandom;

class BlocksVoiceHandler: public VoiceHandler {
public:
  BlocksVoiceHandler(Output* beats_per_second);
  virtual ~BlocksVoiceHandler() { }

  virtual Processor* clone() const override { VITAL_ASSERT(false); return nullptr; }

  void init() override;
  void prepareDestroy();

  void process(int num_samples) override;
  void noteOn(int note, mono_float velocity, int sample, int channel) override;
  void noteOff(int note, mono_float lift, int sample, int channel) override;
  bool shouldAccumulate(Output* output) override;
  void correctToTime(double seconds) override;
  void disableUnnecessaryModSources();
  void disableModSource(const std::string& source);

  output_map& getPolyModulations() override;
  ModulationConnectionBank& getModulationBank() { return modulation_bank_; }
  Wavetable* getWavetable(int index) { return oscillators_[index]->getWavetable(); }
  Sample* getSample() { return nullptr; } // removed
  LineGenerator* getLfoSource(int index) { return &lfo_sources_[index]; }
  Output* getDirectOutput() { return getAccumulatedOutput(direct_output_->output()); }

  Output* note_retrigger() { return &note_retriggered_; }

  Output* midi_offset_output() { return midi_offset_output_; }

  void enableModulationConnection(ModulationConnectionProcessor* processor);
  void disableModulationConnection(ModulationConnectionProcessor* processor);
  CircularQueue<ModulationConnectionProcessor*>& enabledModulationConnection() {
    return enabled_modulation_processors_;
  }

  void addBlock(std::shared_ptr<model::Block> block);
  void connectAll();
  void unplugAll();
  void addModulator(std::shared_ptr<model::Module> module);
  void repositionBlock(Index from, Index to);
  std::shared_ptr<vital::Processor> findProcessorAbove(Index index);

  std::vector<std::shared_ptr<model::Module>> active_modulators_;
private:
  void createNoteArticulation();
  void createOscillators();
  void createModulators();
  void createVoiceOutput();
  void createFilters(Output* keytrack);
  // void createReverbs();
  void setupPolyModulationReadouts();

  std::shared_ptr<SynthModule> createProcessor(std::shared_ptr<model::Block> module);
  std::shared_ptr<EnvelopeModule> createEnvelope(bool audio_rate = false);

  ModulationConnectionBank modulation_bank_;
  CircularQueue<ModulationConnectionProcessor*> enabled_modulation_processors_;
  Output* beats_per_second_;

  Processor* note_from_reference_;
  Output* midi_offset_output_;
  Processor* bent_midi_;
  Processor* current_midi_note_;
  std::shared_ptr<EnvelopeModule> amplitude_envelope_;
  std::shared_ptr<EnvelopeModule> default_amplitude_envelope_;
  Processor* amplitude_;
  Processor* pitch_wheel_;
  Processor* voice_sum_;
  VariableAdd* last_node_;

  std::vector<std::vector<std::shared_ptr<Processor>>> processor_matrix_;
  std::vector<std::shared_ptr<OscillatorModule>> oscillators_;
  std::vector<std::shared_ptr<LfoModule>> lfos_;
  std::vector<std::shared_ptr<EnvelopeModule>> envelopes_;
  std::vector<FilterModule*> filters_;

  FiltersModule* filters_module_;

  ChorusModule* reverb_;

  Output note_retriggered_;

  LineGenerator lfo_sources_[kNumLfos];

  TriggerRandom* random_;
  RandomLfoModule* random_lfos_[kNumRandomLfos];

  LineMap* note_mapping_;
  LineMap* velocity_mapping_;
  LineMap* aftertouch_mapping_;
  LineMap* slide_mapping_;
  LineMap* lift_mapping_;
  LineMap* mod_wheel_mapping_;
  LineMap* pitch_wheel_mapping_;
  ModuleContainer<model::Module> modules_;

  cr::Value* stereo_;
  cr::Multiply* note_percentage_;

  Multiply* output_;
  Multiply* direct_output_;
  Output num_voices_;

  output_map poly_readouts_;
  poly_mask last_active_voice_mask_;
  std::map<std::string, std::vector<std::shared_ptr<SynthModule>>> processors_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlocksVoiceHandler)
};
} // namespace vital
