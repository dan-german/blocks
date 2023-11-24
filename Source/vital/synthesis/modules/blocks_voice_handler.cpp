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

  enabled_modulation_processors_.ensureCapacity(kMaxModulationConnections);
  lfos_.reserve(kNumLfos);

  int rows = 7;
  int columns = 5;

  processor_matrix_.resize(rows);

  for (int i = 0; i < columns; i++) {
    processor_matrix_[i].resize(columns);
  }

  int module_count = 5;
  modules_.spawn({ "osc" }, [](std::string type, int number) { return std::make_shared<model::OscillatorModule>(number); });
  modules_.spawn({ "filter" }, [](std::string type, int number) { return std::make_shared<model::FilterModule>(number); });
  modules_.spawn({ "lfo" }, [](std::string type, int number) { return std::make_shared<model::LFOModule>(number); });
}

std::shared_ptr<model::Module> BlocksVoiceHandler::GetBlock(Index index) {
  for (auto& module : modules_.all) {
    if (module->index == index) {
      return module;
    }
  }
  return nullptr;
}

std::shared_ptr<model::Module> BlocksVoiceHandler::AddModulator(std::string type) {
  std::cout << "adding modulators of type: " << type << std::endl;
  auto module = modules_.get({ type, -1 });
  active_modulators_.push_back(module);
  if (type == "lfo") {
    auto lfo = lfos_[0];
    module->parameters_[0]->val = lfo->control_map_["frequency"];
  }
  return module;
}

std::shared_ptr<model::Module> BlocksVoiceHandler::AddBlock(std::string type, Index index) {
  auto module = modules_.get({ type, -1 });
  module->index = index;
  createProcessor(module);

  for (int column = 0; column < processor_matrix_.size(); column++) {
    for (int row = 0; row < processor_matrix_[column].size(); row++) {
      auto processor = processor_matrix_[column][row];
      if (processor != nullptr) {
        voice_sum_->unplug(processor_matrix_[column][row].get());
      }
    }
  }

  Processor* current = nullptr;
  Processor* target = nullptr;
  for (int column = 0; column < processor_matrix_.size(); column++) {
    for (int row = 0; row < processor_matrix_[column].size(); row++) {
      auto processor = processor_matrix_[column][row];
      if (processor != nullptr) {
        if (current) {
          processor->plug(current, 0);
        }

        current = processor.get();
      }
    }
    last_node_->plug(current, index.column);
  }

  return module;
}

void BlocksVoiceHandler::init() {
  createNoteArticulation();
  createOscillators();
  createModulators();
  createVoiceOutput();

  last_node_ = new VariableAdd(5);
  addProcessor(last_node_);
  voice_sum_ = new Add();
  output_->plug(last_node_, 0);
  output_->plug(amplitude_, 1);

  direct_output_->plug(amplitude_, 1);

  addProcessor(voice_sum_);
  addProcessor(output_);
  addProcessor(direct_output_);

  Output* macros[kNumMacros];
  for (int i = 0; i < kNumMacros; ++i)
    macros[i] = createMonoModControl("macro_control_" + std::to_string(i + 1));

  setVoiceKiller(amplitude_->output());

  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor.get();

    processor->plug(reset(), ModulationConnectionProcessor::kReset);

    std::string number = std::to_string(i + 1);
    std::string amount_name = "modulation_" + number + "_amount";
    Output* modulation_amount = createPolyModControl(amount_name);
    processor->plug(modulation_amount, ModulationConnectionProcessor::kModulationAmount);
    processor->initializeBaseValue(data_->controls[amount_name]);

    Output* modulation_power = createPolyModControl("modulation_" + number + "_power");
    processor->plug(modulation_power, ModulationConnectionProcessor::kModulationPower);

    addProcessor(processor);
    addSubmodule(processor);
    processor->enable(false);
  }

  VoiceHandler::init();

  setupPolyModulationReadouts();

  for (int i = 0; i < kNumMacros; ++i) {
    std::string name = "macro_control_" + std::to_string(i + 1);
    data_->mod_sources[name] = macros[i];
    createStatusOutput(name, macros[i]);
  }

  for (int i = 0; i < kNumRandomLfos; ++i) {
    std::string name = "random_" + std::to_string(i + 1);
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
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor.get();
    std::string number = std::to_string(i + 1);
    Output* source_output = processor->output(ModulationConnectionProcessor::kModulationSource);
    Output* pre_scale_output = processor->output(ModulationConnectionProcessor::kModulationPreScale);
    createStatusOutput(modulation_source_prefix + number, source_output);
    createStatusOutput(modulation_amount_prefix + number, pre_scale_output);
  }
}

void BlocksVoiceHandler::prepareDestroy() {
  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor.get();
    removeProcessor(processor);
  }
}

void BlocksVoiceHandler::createFilters(Output* keytrack) {
  for (int i = 0; i < 5; i++) {
    auto name = "filter_" + std::to_string(i + 1);
    auto filter = std::make_shared<FilterModule>(name);

    filter->plug(reset(), FilterModule::kReset);
    filter->plug(bent_midi_, FilterModule::kMidi);

    addSubmodule(filter.get());
    addProcessor(filter.get());

    processors_["filter"].push_back(filter);
  }
}

std::shared_ptr<SynthModule> BlocksVoiceHandler::createProcessor(std::shared_ptr<model::Module> module) {
  auto index = module->index;
  std::shared_ptr<SynthModule> processor;
  auto name = module->name;

  if (module->id.type == "osc") {
    auto osc = oscillators_[0];
    processor = osc;
    module->parameters_[0]->val = osc->control_map_["transpose"];
    module->parameters_[1]->val = osc->control_map_["tune"];
    module->parameters_[2]->val = osc->control_map_["unison_voices"];
    module->parameters_[3]->val = osc->control_map_["unison_detune"];
    module->parameters_[4]->val = osc->control_map_["level"];
    module->parameters_[5]->val = osc->control_map_["pan"];
    oscillators_.erase(oscillators_.begin());
  } else if (module->id.type == "filter") {
    auto filter = std::make_shared<FilterModule>(name);
    addProcessor(filter.get());
    addSubmodule(filter.get());
    filter->init();
    module->parameters_[0]->val = filter->control_map_["style"];
    module->parameters_[1]->val = filter->control_map_["cutoff"];
    processor = filter;
    filter->plug(reset(), FilterModule::kReset);
    filter->plug(bent_midi_, FilterModule::kMidi);
    filter->plug(note_from_reference_->output(), FilterModule::kKeytrack);
    processors_["filter"].push_back(filter);
    filter->control_map_["on"]->set(1.0f);
  }

  processor_matrix_[index.column][index.row] = processor;
  processors_[module->id.type].push_back(processor);
  return processor;
}

void BlocksVoiceHandler::createOscillators() {
  std::string type = "osc";
  for (int i = 0; i < 5; i++) {
    auto name = type + "_" + std::to_string(i + 1);
    auto osc = std::make_shared<OscillatorModule>(name);

    osc->plug(reset(), OscillatorModule::kReset);
    osc->plug(retrigger(), OscillatorModule::kRetrigger);
    osc->plug(bent_midi_, OscillatorModule::kMidi);
    osc->plug(active_mask(), OscillatorModule::kActiveVoices);
    addSubmodule(osc.get());
    addProcessor(osc.get());

    processors_[type].push_back(osc);
    oscillators_.push_back(osc);
  }
}

void BlocksVoiceHandler::createModulators() {
  for (int i = 0; i < kNumLfos; ++i) {
    lfo_sources_[i].setLoop(false);
    lfo_sources_[i].initTriangle();
    std::string prefix = std::string("lfo_") + std::to_string(i + 1);
    auto lfo = std::make_shared<LfoModule>(prefix, &lfo_sources_[i], beats_per_second_);
    addSubmodule(lfo.get());
    addProcessor(lfo.get());
    lfos_.push_back(lfo);
    lfo->plug(retrigger(), LfoModule::kNoteTrigger);
    lfo->plug(note_count(), LfoModule::kNoteCount);
    lfo->plug(bent_midi_, LfoModule::kMidi);

    data_->mod_sources[prefix] = lfo->output(LfoModule::kValue);
    createStatusOutput(prefix, lfo->output(LfoModule::kValue));
    createStatusOutput(prefix + "_phase", lfo->output(LfoModule::kOscPhase));
    createStatusOutput(prefix + "_frequency", lfo->output(LfoModule::kOscFrequency));
  }

  for (int i = 0; i < kNumEnvelopes; ++i) {
    std::string prefix = std::string("env_") + std::to_string(i + 1);
    EnvelopeModule* envelope = new EnvelopeModule(prefix, i == 0);
    envelope->plug(retrigger(), EnvelopeModule::kTrigger);
    addSubmodule(envelope);
    addProcessor(envelope);
    envelopes_[i] = envelope;

    data_->mod_sources[prefix] = envelope->output();
    createStatusOutput(prefix, envelope->output(EnvelopeModule::kValue));
    createStatusOutput(prefix + "_phase", envelope->output(EnvelopeModule::kPhase));
  }

  random_ = new TriggerRandom();
  random_->plug(retrigger());
  addProcessor(random_);

  for (int i = 0; i < kNumRandomLfos; ++i) {
    std::string name = "random_" + std::to_string(i + 1);
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

  amplitude_envelope_ = envelopes_[0];
  amplitude_envelope_->setControlRate(false);

  Processor* control_amplitude = new SmoothMultiply();
  control_amplitude->plug(amplitude_envelope_->output(Envelope::kValue), SmoothMultiply::kAudioRate);
  control_amplitude->plug(amplitude, SmoothMultiply::kControlRate);
  control_amplitude->plug(reset(), SmoothMultiply::kReset);

  amplitude_ = new Square();
  amplitude_->plug(control_amplitude);

  addProcessor(control_amplitude);
  addProcessor(amplitude_);
}

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
    for (auto& status_source : data_->status_outputs)
      status_source.second->update(last_active_voice_mask_);

    for (ModulationConnectionProcessor* processor : enabled_modulation_processors_) {
      poly_float* buffer = processor->output()->buffer;
      if (processor->isControlRate() || processor->isPolyphonicModulation()) {
        poly_float masked_value = buffer[0] & last_active_voice_mask_;
        buffer[0] = masked_value + utils::swapVoices(masked_value);
      } else {
        for (int i = 0; i < num_samples; ++i) {
          poly_float masked_value = buffer[i] & last_active_voice_mask_;
          buffer[i] = masked_value + utils::swapVoices(masked_value);
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
  if (output->owner == amplitude_envelope_)
    return false;

  return VoiceHandler::shouldAccumulate(output);
}

void BlocksVoiceHandler::correctToTime(double seconds) {
  for (int i = 0; i < kNumLfos; ++i)
    lfos_[i]->correctToTime(seconds);

  for (int i = 0; i < kNumRandomLfos; ++i)
    random_lfos_[i]->correctToTime(seconds);
}

void BlocksVoiceHandler::disableUnnecessaryModSources() {
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
} // namespace vital