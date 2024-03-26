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

#include "vital/synthesis/modules/blocks_voice_handler.h"
#include "vital/synthesis/modulators/envelope.h"
#include "vital/synthesis/modules/envelope_module.h"
#include "vital/synthesis/modules/filters_module.h"
#include "vital/synthesis/modulators/line_map.h"
#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/utilities/portamento_slope.h" 
#include "vital/synthesis/modules/random_lfo_module.h" 
#include "vital/common/synth_constants.h"
#include "vital/synthesis/modules/lfo_module.h" 
#include "vital/synthesis/modulators/trigger_random.h" 
#include "vital/synthesis/modulators/trigger_random.h" 
#include "vital/synthesis/modules/modulation_connection_processor.h"
#include "vital/synthesis/effects/reverb.h"
#include "module_new.h"
#include "oscillator_module_new.h"
#include "filter_module_new.h"
#include "lfo_module_new.h"
#include "blocks_voice_handler.h"
#include "vital/synthesis/modules/reverb_module.h"
#include "vital/synthesis/modules/delay_module.h"
#include "vital/synthesis/modules/distortion_module.h"
#include "vital/synthesis/modules/chorus_module.h"
#include "vital/synthesis/modules/flanger_module.h"
#include "vital/synthesis/modules/phaser_module.h"
#include "vital/synthesis/utilities/value_switch.h"
#include "vital/synthesis/modules/sample_module.h"

namespace vital {

BlocksVoiceHandler::BlocksVoiceHandler(Output* beats_per_second):
  VoiceHandler(0, kMaxPolyphony), beats_per_second_(beats_per_second),
  note_from_reference_(nullptr), midi_offset_output_(nullptr),
  bent_midi_(nullptr), current_midi_note_(nullptr), amplitude_envelope_(nullptr), amplitude_(nullptr),
  pitch_wheel_(nullptr), filters_module_(nullptr), lfos_(), envelopes_(), lfo_sources_(), random_(nullptr),
  random_lfos_(), note_mapping_(nullptr), velocity_mapping_(nullptr), aftertouch_mapping_(nullptr),
  slide_mapping_(nullptr), lift_mapping_(nullptr), mod_wheel_mapping_(nullptr),
  pitch_wheel_mapping_(nullptr), stereo_(nullptr), note_percentage_(nullptr), last_active_voice_mask_(0) {
  output_ = new Multiply();
  registerOutput(output_->output());

  direct_output_ = new Multiply();
  registerOutput(direct_output_->output());

  note_from_reference_ = new cr::Add();
  midi_offset_output_ = registerControlRateOutput(note_from_reference_->output(), true);

  enabled_modulation_processors_.ensureCapacity(kMaxModulationConnections * 2); // * 2 for the default envelope modultion processors. todo - at least calculate the exact amount
  lfos_.reserve(kNumLfos);
  envelopes_.reserve(kNumEnvelopes);

  int rows = 7;
  int columns = 5;

  processor_matrix_.resize(columns);

  for (int i = 0; i < columns; i++) {
    processor_matrix_[i].resize(rows);
  }
}

void BlocksVoiceHandler::removeModulator(int index, std::string type, std::string name) {
  auto modulator = active_modulators_[index];
  active_modulators_.erase(active_modulators_.begin() + index);
  active_modulators_map_[name] = nullptr;
  processor_pool_[type].push_back(modulator);
}

void BlocksVoiceHandler::addModulator(std::shared_ptr<model::Module> modulator) {
  auto type = modulator->id.type;

  SynthModule* processor = processor_pool_[type][0];
  processor_pool_[type].erase(processor_pool_[type].begin());

  processor->setModule(modulator);
  active_modulators_.push_back(processor);
  active_modulators_map_[modulator->name] = processor;
}

void BlocksVoiceHandler::repositionBlock(Index from, Index to) {
  processor_matrix_[to.column][to.row] = processor_matrix_[from.column][from.row];
  processor_matrix_[from.column][from.row] = nullptr;
}

void BlocksVoiceHandler::connectAll() {
  Processor* current = nullptr;
  Processor* target = nullptr;
  for (int column = 0; column < processor_matrix_.size(); column++) {
    for (int row = 0; row < processor_matrix_[column].size(); row++) {
      if (auto processor = processor_matrix_[column][row]) {
        if (current) {
          processor->plug(current);
        }

        current = processor;
      }
    }

    if (current) {
      column_nodes_[column]->plug(current);
    }
    current = nullptr;
  }
}

vital::Processor* BlocksVoiceHandler::findProcessorAbove(Index index) {
  for (int i = index.row - 1; i >= 0; i--) {
    auto processor = processor_matrix_[index.column][i];
    if (processor != nullptr) {
      return processor;
    }
  }
  return nullptr;
}

// void BlocksVoiceHandler::unplugAll() {
//   for (int column = 0; column < processor_matrix_.size(); column++) {
//     for (int row = 0; row < processor_matrix_[column].size(); row++) {
//       auto processor = processor_matrix_[column][row];
//       if (processor != nullptr) {
//         column_nodes_[column]->unplug(processor.get());

//         if (auto processor_above = findProcessorAbove({ row, column })) {
//           processor->unplug(processor_above.get());
//         }
//       }
//     }
//   }
// }

void BlocksVoiceHandler::unplugAll() {
  for (auto processor : active_processors_) {
    processor->unplug(processor->input()->source);
  }

  for (auto node : column_nodes_) {
    node->unplug(node->input()->source);
  }
}

void BlocksVoiceHandler::init() {
  createNoteArticulation();
  createOscillators();
  createReverbs();
  createDistortions();
  createChoruses();
  createFlangers();
  createNoises();
  createPhasers();
  createDelays();
  createModulators();
  createFilters(note_from_reference_->output());
  createVoiceOutput();

  master_node_ = new VariableAdd(5);
  addProcessor(master_node_);
  // voice_sum_ = new Add();
  output_->plug(master_node_, 0);
  output_->plug(amplitude_, 1);

  direct_output_->plug(amplitude_, 1);

  // addProcessor(voice_sum_);
  addProcessor(output_);
  addProcessor(direct_output_);

  Output* macros[kNumMacros];
  for (int i = 0; i < kNumMacros; ++i)
    macros[i] = createMonoModControl("macro_control_" + std::to_string(i + 1));

  setVoiceKiller(master_node_->output());

  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor;
    processor->plug(reset(), ModulationConnectionProcessor::kReset);
    std::string number = std::to_string(i + 1);
    std::string amount_name = "modulation_" + number + "_amount";
    Output* modulation_amount = createPolyModControl2({ .name = "amount", .min = -1.0f });
    processor->plug(modulation_amount, ModulationConnectionProcessor::kModulationAmount);
    processor->initializeBaseValue(data_->controls[amount_name]);
    Output* modulation_power = createPolyModControl2({ .name = "power", .min = -10.0f, .max = 10.0f });
    processor->plug(modulation_power, ModulationConnectionProcessor::kModulationPower);
    addProcessor(processor);
    addSubmodule(processor);
    processor->enable(false);
  }

  VoiceHandler::init();


  // disable all processors
  for (const auto& pair : processor_pool_) {
    const std::string& key = pair.first;
    const std::vector<SynthModule*>& modules = pair.second;
    for (const auto& modulePtr : modules) {
      modulePtr->enable(false);
    }
  }

  setupPolyModulationReadouts();

  for (int i = 0; i < kNumMacros; ++i) {
    std::string name = "macro_control_" + std::to_string(i + 1);
    data_->mod_sources[name] = macros[i];
    createStatusOutput(name, macros[i]);
  }

  for (int i = 0; i < kNumRandomLfos; ++i) {
    std::string name = "random_";
    data_->mod_sources[name] = random_lfos_[i]->output();
    createStatusOutput(name, random_lfos_[i]->output());
  }

  data_->mod_sources["random"] = random_->output();
  data_->mod_sources["stereo"] = stereo_->output();

  createStatusOutput("random", random_->output());
  createStatusOutput("stereo", stereo_->output());
  // createStatusOutput("sample_phase", producers_->samplePhaseOutput());
  createStatusOutput("num_voices", &num_voices_);

  std::string modulation_source_prefix = "modulation_source_";
  std::string modulation_amount_prefix = "modulation_amount_";
  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor;
    std::string number = std::to_string(i + 1);
    Output* source_output = processor->output(ModulationConnectionProcessor::kModulationSource);
    Output* pre_scale_output = processor->output(ModulationConnectionProcessor::kModulationPreScale);
    createStatusOutput(modulation_source_prefix + number, source_output);
    createStatusOutput(modulation_amount_prefix + number, pre_scale_output);
  }
  initializeDefaultAmpEnvs();

  for (auto node : column_nodes_) {
    master_node_->plugNext(node);
  }
}

void BlocksVoiceHandler::initializeDefaultAmpEnvs() {
  for (auto processor : processors_with_default_env) {
    // continue;
    auto source = default_amp_env_->output();
    auto modulation_connection_processor = new ModulationConnectionProcessor(1000);
    addProcessor(modulation_connection_processor);
    modulation_connection_processor->plug(default_amp_env_->output(), ModulationConnectionProcessor::kModulationInput);
    modulation_connection_processor->init();
    processor_default_env_mp_map_[processor] = modulation_connection_processor;
  }
}

void BlocksVoiceHandler::setDefaultAmpEnvState(std::string target_name, bool enable) {
  auto osc = active_processor_map_[target_name];
  processor_default_env_mp_map_[osc]->enable(enable);
}

void BlocksVoiceHandler::disconnectAllDefaultEnvs() {
  for (auto osc : processors_with_default_env) {
    auto processor = processor_default_env_mp_map_[osc];
    auto destination = osc->getPolyModulationDestination("amp env destination");
    destination->unplug(processor);
    osc->getPolyModulationSwitch("amp env destination")->set(0.0f);
    setInactiveNonaccumulatedOutput(destination->output());
    disableModulationConnection(processor);
  }
}

void BlocksVoiceHandler::connectAllDefaultEnvs() {
  for (auto osc : processors_with_default_env) {
    auto processor = processor_default_env_mp_map_[osc];
    auto destination = osc->getPolyModulationDestination("amp env destination");
    processor->setDestinationScale(1.0f);
    processor->setPolyphonicModulation(true);
    processor->control_map_["amount"]->set(1.0f);
    destination->plugNext(processor);
    processor->process(1);
    destination->process(1);
    osc->getPolyModulationSwitch("amp env destination")->set(1.0f);
    setActiveNonaccumulatedOutput(destination->output());
    enableModulationConnection(processor);
  }
}

void BlocksVoiceHandler::prepareDestroy() {
  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor;
    removeProcessor(processor);
  }
}

void BlocksVoiceHandler::createFilters(Output* keytrack) {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto filter = new FilterModule("filter");
    filter->plug(reset(), FilterModule::kReset);
    filter->plug(bent_midi_, FilterModule::kMidi);
    filter->plug(keytrack, FilterModule::kKeytrack);
    addSubmodule(filter);
    addProcessor(filter);
    processor_pool_["filter"].push_back(filter);
  }
}

void BlocksVoiceHandler::createDistortions() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto distortion = new DistortionModule();
    distortion->plug(reset(), DistortionModule::kReset);
    addSubmodule(distortion);
    addProcessor(distortion);
    processor_pool_["drive"].push_back(distortion);
  }
}

void BlocksVoiceHandler::createChoruses() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto chorus = new ChorusModule(beats_per_second_);
    chorus->plug(reset(), ChorusModule::kReset);
    addSubmodule(chorus);
    addProcessor(chorus);
    processor_pool_["chorus"].push_back(chorus);
  }
}

void BlocksVoiceHandler::createPhasers() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto phaser = new PhaserModule(beats_per_second_);
    addSubmodule(phaser);
    addProcessor(phaser);
    processor_pool_["phaser"].push_back(phaser);
  }
}

void BlocksVoiceHandler::createFlangers() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto flanger = new FlangerModule(beats_per_second_);
    addSubmodule(flanger);
    addProcessor(flanger);
    processor_pool_["flanger"].push_back(flanger);
  }
}

void BlocksVoiceHandler::createNoises() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto noise = new SampleModule();
    addSubmodule(noise);
    addProcessor(noise);
    noise->plug(reset(), SampleModule::kReset);
    noise->plug(note_count(), SampleModule::kNoteCount);
    noise->plug(bent_midi_, SampleModule::kMidi);
    processor_pool_["noise"].push_back(noise);
    processors_with_default_env.push_back(noise);
  }
}


void BlocksVoiceHandler::createReverbs() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto reverb = new ReverbModule();
    addSubmodule(reverb);
    addProcessor(reverb);
    reverb->plug(reset(), ReverbModule::kReset);
    reverb->enable(false);
    processor_pool_["reverb"].push_back(reverb);
  }
}

void BlocksVoiceHandler::createDelays() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto delay = new DelayModule(beats_per_second_);
    delay->plug(reset(), DelayModule::kReset);
    addSubmodule(delay);
    addProcessor(delay);
    delay->enable(false);
    processor_pool_["delay"].push_back(delay);
  }
}

void BlocksVoiceHandler::removeBlock(Index index, std::shared_ptr<model::Block> block) {
  auto processor = processor_matrix_[index.column][index.row];
  processor->enable(false);

  if (processor->control_map_.count("on"))
    processor->control_map_["on"]->set(0.0f);

  processor_pool_[block->id.type].push_back(processor);
  processor_matrix_[index.column][index.row] = nullptr;
}

void BlocksVoiceHandler::addBlock(std::shared_ptr<model::Block> block) {
  createProcessorForBlock(block);
}

SynthModule* BlocksVoiceHandler::createProcessorForBlock(std::shared_ptr<model::Block> module) {
  SynthModule* processor = processor_pool_[module->id.type][0];
  processor_pool_[module->id.type].erase(processor_pool_[module->id.type].begin());
  processor->enable(true);
  if (processor->control_map_.count("on")) {
    processor->control_map_["on"]->set(1.0f);
  }

  for (auto& pair : module->parameter_map_) {
    module->parameter_map_[pair.first]->value_processor = processor->control_map_[pair.first];
  }

  auto index = module->index;
  processor_matrix_[index.column][index.row] = processor;
  active_processor_map_[module->name] = processor;
  active_processors_.push_back(processor);
  processor->setModule(module);
  return processor;
}

void BlocksVoiceHandler::createOscillators() {
  std::string type = "osc";
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto osc = new OscillatorModule();

    osc->plug(reset(), OscillatorModule::kReset);
    osc->plug(retrigger(), OscillatorModule::kRetrigger);
    osc->plug(bent_midi_, OscillatorModule::kMidi);
    osc->plug(active_mask(), OscillatorModule::kActiveVoices);
    osc->enable(false);
    addSubmodule(osc);
    addProcessor(osc);

    processor_pool_[type].push_back(osc);
    oscillators_.push_back(osc);
    processors_with_default_env.push_back(osc);
  }
}

void BlocksVoiceHandler::clear() {
  unplugAll();

  active_processors_.clear();
  active_processor_map_.clear();

  active_modulators_.clear();
  active_modulators_map_.clear();
}

void BlocksVoiceHandler::createModulators() {
  for (int i = 0; i < kNumLfos; ++i) {
    lfo_sources_[i].setLoop(false);
    lfo_sources_[i].initSin();
    std::string prefix = std::string("lfo");
    auto lfo = new LfoModule(prefix, &lfo_sources_[i], beats_per_second_);
    lfo->enable(false);
    addSubmodule(lfo);
    addProcessor(lfo);
    lfos_.push_back(lfo);
    processor_pool_["lfo"].push_back(lfo);
    lfo->plug(retrigger(), LfoModule::kNoteTrigger);
    lfo->plug(note_count(), LfoModule::kNoteCount);
    lfo->plug(bent_midi_, LfoModule::kMidi);

    data_->mod_sources[prefix + std::to_string(i + 1)] = lfo->output(LfoModule::kValue);
    createStatusOutput(prefix, lfo->output(LfoModule::kValue));
    createStatusOutput(prefix + "_phase", lfo->output(LfoModule::kOscPhase));
    createStatusOutput(prefix + "_frequency", lfo->output(LfoModule::kOscFrequency));
  }

  for (int i = 0; i < kNumEnvelopes; ++i) {
    auto envelope = createEnvelope();
    envelopes_.push_back(envelope);
    processor_pool_["envelope"].push_back(envelope);
    createStatusOutput("env", envelope->output(EnvelopeModule::kValue));
    createStatusOutput("env_phase", envelope->output(EnvelopeModule::kPhase));
    data_->mod_sources["env"] = envelope->output();
  }

  default_amp_env_ = createEnvelope();
  default_amp_env_->enable(true);

  random_ = new TriggerRandom();
  random_->plug(retrigger());
  addProcessor(random_);

  for (int i = 0; i < kNumRandomLfos; ++i) {
    std::string name = "random";
    random_lfos_[i] = new RandomLfoModule(name, beats_per_second_);
    random_lfos_[i]->plug(retrigger(), RandomLfoModule::kNoteTrigger);
    random_lfos_[i]->plug(bent_midi_, RandomLfoModule::kMidi);
    addSubmodule(random_lfos_[i]);
    addProcessor(random_lfos_[i]);
  }

  stereo_ = new cr::Value(constants::kLeftOne);
  addIdleMonoProcessor(stereo_);

  data_->mod_sources["note"] = note_percentage_->output();
  data_->mod_sources["note_in_octave"] = note_in_octave();
  data_->mod_sources["aftertouch"] = aftertouch();
  data_->mod_sources["velocity"] = velocity();
  data_->mod_sources["slide"] = slide();
  data_->mod_sources["lift"] = lift();
  data_->mod_sources["mod_wheel"] = mod_wheel();
  data_->mod_sources["pitch_wheel"] = pitch_wheel_percent();

  createStatusOutput("note", note_percentage_->output());
  createStatusOutput("note_in_octave", note_in_octave());
  createStatusOutput("aftertouch", aftertouch());
  createStatusOutput("velocity", velocity());
  createStatusOutput("slide", slide());
  createStatusOutput("lift", lift());
  createStatusOutput("mod_wheel", mod_wheel());
  createStatusOutput("pitch_wheel", pitch_wheel_percent());
}

EnvelopeModule* BlocksVoiceHandler::createEnvelope(bool audio_rate) {
  auto envelope = new EnvelopeModule(audio_rate);
  envelope->enable(false);
  envelope->plug(retrigger(), EnvelopeModule::kTrigger);
  addSubmodule(envelope);
  addProcessor(envelope);
  return envelope;
}

void BlocksVoiceHandler::createNoteArticulation() {
  Output* portamento = createPolyModControl("portamento_time");
  Output* portamento_slope = createPolyModControl("portamento_slope");
  Value* portamento_force = createBaseControl("portamento_force");
  Value* portamento_scale = createBaseControl("portamento_scale");

  current_midi_note_ = new PortamentoSlope();
  current_midi_note_->plug(last_note(), PortamentoSlope::kSource);
  current_midi_note_->plug(note(), PortamentoSlope::kTarget);
  current_midi_note_->plug(portamento_force, PortamentoSlope::kPortamentoForce);
  current_midi_note_->plug(portamento_scale, PortamentoSlope::kPortamentoScale);
  current_midi_note_->plug(portamento, PortamentoSlope::kRunSeconds);
  current_midi_note_->plug(portamento_slope, PortamentoSlope::kSlopePower);
  current_midi_note_->plug(voice_event(), PortamentoSlope::kReset);
  current_midi_note_->plug(note_pressed(), PortamentoSlope::kNumNotesPressed);
  setVoiceMidi(current_midi_note_->output());
  addProcessor(current_midi_note_);

  Output* pitch_bend_range = createPolyModControl("pitch_bend_range");
  Output* voice_tune = createPolyModControl("voice_tune");
  Output* voice_transpose = createPolyModControl("voice_transpose");
  cr::Multiply* pitch_bend = new cr::Multiply();
  pitch_bend->plug(pitch_wheel(), 0);
  pitch_bend->plug(pitch_bend_range, 1);
  bent_midi_ = new cr::VariableAdd();
  bent_midi_->plugNext(current_midi_note_);
  bent_midi_->plugNext(pitch_bend);
  bent_midi_->plugNext(local_pitch_bend());
  bent_midi_->plugNext(voice_tune);
  bent_midi_->plugNext(voice_transpose);

  static const cr::Value max_midi_invert(1.0f / (kMidiSize - 1));
  note_percentage_ = new cr::Multiply();
  note_percentage_->plug(&max_midi_invert, 0);
  note_percentage_->plug(bent_midi_, 1);
  addProcessor(note_percentage_);

  static const cr::Value reference_adjust(-kMidiTrackCenter);
  note_from_reference_->plug(&reference_adjust, 0);
  note_from_reference_->plug(bent_midi_, 1);
  addProcessor(note_from_reference_);

  addProcessor(pitch_bend);
  addProcessor(bent_midi_);
}

void BlocksVoiceHandler::createVoiceOutput() {
  Output* velocity_track_amount = createPolyModControl("velocity_track");
  cr::Interpolate* velocity_track_mult = new cr::Interpolate();
  velocity_track_mult->plug(&constants::kValueOne, Interpolate::kFrom);
  velocity_track_mult->plug(velocity(), Interpolate::kTo);
  velocity_track_mult->plug(velocity_track_amount, Interpolate::kFractional);
  addProcessor(velocity_track_mult);

  Output* voice_amplitude = createPolyModControl("voice_amplitude");
  cr::Multiply* amplitude = new cr::Multiply();
  amplitude->plug(velocity_track_mult, 0);
  amplitude->plug(voice_amplitude, 1);
  addProcessor(amplitude);

  Value* val = new Value(0.9f);
  addProcessor(val);

  amplitude_ = new Square();
  amplitude_->plug(val);
  addProcessor(amplitude_);

  for (int i = 0; i < Constants::columns; i++) {
    column_nodes_.push_back(new ColumnMasterModule());
    addProcessor(column_nodes_[i]);
  }
}

int please = 0;
void BlocksVoiceHandler::process(int num_samples) {
  poly_mask reset_mask = reset()->trigger_mask;
  if (reset_mask.anyMask())
    resetFeedbacks(reset_mask);

  VoiceHandler::process(num_samples);
  int num_voices = getNumActiveVoices();
  num_voices_.buffer[0] = num_voices;
  note_retriggered_.clearTrigger();

  if (num_voices == 0) {
    for (auto& status_source : data_->status_outputs)
      status_source.second->clear();
  } else {
    last_active_voice_mask_ = getCurrentVoiceMask();
    for (auto& status_source : data_->status_outputs) {
      status_source.second->update(last_active_voice_mask_);
    }

    for (ModulationConnectionProcessor* processor : enabled_modulation_processors_) {
      poly_float* buffer = processor->output()->buffer;
      if (processor->isControlRate() || processor->isPolyphonicModulation()) {
        poly_float masked_value = buffer[0] & last_active_voice_mask_;
        buffer[0] = masked_value;
      } else {
        for (int i = 0; i < num_samples; ++i) {
          poly_float masked_value = buffer[i] & last_active_voice_mask_;
          buffer[i] = masked_value;
        }
      }
    }
  }
}

void BlocksVoiceHandler::noteOn(int note, mono_float velocity, int sample, int channel) {
  if (getNumPressedNotes() < polyphony() || !legato())
    note_retriggered_.trigger(constants::kFullMask, note, sample);
  VoiceHandler::noteOn(note, velocity, sample, channel);
}

void BlocksVoiceHandler::noteOff(int note, mono_float lift, int sample, int channel) {
  if (getNumPressedNotes() > polyphony() && isNotePlaying(note) && !legato())
    note_retriggered_.trigger(constants::kFullMask, note, sample);

  VoiceHandler::noteOff(note, lift, sample, channel);
}

bool BlocksVoiceHandler::shouldAccumulate(Output* output) {
  // if (output->owner == amplitude_envelope_) // what is this for? 
  //   return true;

  return VoiceHandler::shouldAccumulate(output);
}

void BlocksVoiceHandler::correctToTime(double seconds) {
  for (int i = 0; i < kNumLfos; ++i)
    lfos_[i]->correctToTime(seconds);

  for (int i = 0; i < kNumRandomLfos; ++i)
    random_lfos_[i]->correctToTime(seconds);
}

void BlocksVoiceHandler::disableUnnecessaryModSources() {
  return;
  for (int i = 0; i < kNumLfos; ++i)
    lfos_[i]->enable(false);

  for (int i = 1; i < kNumEnvelopes; ++i)
    envelopes_[i]->enable(false);

  for (int i = 0; i < kNumRandomLfos; ++i)
    random_lfos_[i]->enable(false);

  random_->enable(false);
}

void BlocksVoiceHandler::disableModSource(const std::string& source) {
  if (source != "env_1")
    getModulationSource(source)->owner->enable(false);
}

void BlocksVoiceHandler::enableModulationConnection(ModulationConnectionProcessor* processor) {
  enabled_modulation_processors_.push_back(processor);
}

void BlocksVoiceHandler::disableModulationConnection(ModulationConnectionProcessor* processor) {
  enabled_modulation_processors_.remove(processor);
}

void BlocksVoiceHandler::setupPolyModulationReadouts() {
  output_map& poly_mods = SynthModule::getPolyModulations();

  for (auto& mod : poly_mods)
    poly_readouts_[mod.first] = registerControlRateOutput(mod.second, mod.second->owner->enabled());
}

output_map& BlocksVoiceHandler::getPolyModulations() {
  return poly_readouts_;
}

void BlocksVoiceHandler::allSoundsOff() {
  VoiceHandler::allSoundsOff();
  for (auto& processor : active_processors_) {
    processor->hardReset();
  }
}
} // namespace vital