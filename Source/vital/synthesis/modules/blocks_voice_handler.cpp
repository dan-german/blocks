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

  std::shared_ptr<SynthModule> processor = processor_pool_[type][0];
  processor_pool_[type].erase(processor_pool_[type].begin());

  processor->setModule(modulator);
  active_modulators_.push_back(processor);
  active_modulators_map_[modulator->name] = processor;
}

void BlocksVoiceHandler::repositionBlock(Index from, Index to) {
  unplugAll();
  std::cout << "from: " << from.column << ", " << from.row << " | to: " << to.column << ", " << to.row << std::endl;
  processor_matrix_[to.column][to.row] = processor_matrix_[from.column][from.row];
  processor_matrix_[from.column][from.row] = nullptr;
  connectAll();
}

void BlocksVoiceHandler::removeBlock(Index index, std::shared_ptr<model::Block> block) {
  // auto processor = graphManager.getModuleProcessor(index);
  // listeners.removeFirstMatchingValue(processor.get());
  // graphManager.removeNode(processor, index);
  // pool->retire(processor);

  unplugAll();
  auto processor = processor_matrix_[index.column][index.row];

  if (processor->control_map_.count("on"))
    processor->control_map_["on"]->set(0.0f);

  // if (block->id.type == "osc") {
  // } else if (block->id.type == "filter") {
    // auto filter = std::static_pointer_cast<FilterModule>(processor);
    // filter->control_map_["on"]->set(0.0f); 
    // processors_["filter"].push_back(filter);
  // }

  processor_pool_[block->id.type].push_back(processor);
  processor_matrix_[index.column][index.row] = nullptr;
  connectAll();
}

int y = 0;

void BlocksVoiceHandler::addBlock(std::shared_ptr<model::Block> block) {
  createProcessor(block);
  auto first_osc = oscillators_[0];
  unplugAll();
  connectAll();
}

void BlocksVoiceHandler::connectAll() {
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

    if (current) {
      last_node_->plug(current, column);
    }
    current = nullptr;
  }
}

std::shared_ptr<vital::Processor> BlocksVoiceHandler::findProcessorAbove(Index index) {
  for (int i = index.row - 1; i >= 0; i--) {
    auto processor = processor_matrix_[index.column][i];
    if (processor != nullptr) {
      return processor;
    }
  }
  return nullptr;
}

void BlocksVoiceHandler::unplugAll() {
  for (int column = 0; column < processor_matrix_.size(); column++) {
    for (int row = 0; row < processor_matrix_[column].size(); row++) {
      auto processor = processor_matrix_[column][row];
      if (processor != nullptr) {
        last_node_->unplug(processor.get());

        if (auto processor_above = findProcessorAbove({ row, column })) {
          processor->unplug(processor_above.get());
        }
      }
    }
  }

  // last_node_->hardReset();
}

void BlocksVoiceHandler::init() {
  createNoteArticulation();
  createOscillators();
  createReverbs();
  createDistortions();
  createChoruses();
  createDelays();
  createModulators();
  createFilters(note_from_reference_->output());
  createVoiceOutput();

  last_node_ = new VariableAdd(5);
  addProcessor(last_node_);
  // voice_sum_ = new Add();
  output_->plug(last_node_, 0);
  output_->plug(amplitude_, 1);

  direct_output_->plug(amplitude_, 1);

  // addProcessor(voice_sum_);
  addProcessor(output_);
  addProcessor(direct_output_);

  Output* macros[kNumMacros];
  for (int i = 0; i < kNumMacros; ++i)
    macros[i] = createMonoModControl("macro_control_" + std::to_string(i + 1));

  setVoiceKiller(last_node_->output());

  for (int i = 0; i < vital::kMaxModulationConnections; ++i) {
    ModulationConnectionProcessor* processor = modulation_bank_.atIndex(i)->modulation_processor.get();

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
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto filter = std::make_shared<FilterModule>("filter");
    filter->plug(reset(), FilterModule::kReset);
    filter->plug(bent_midi_, FilterModule::kMidi);
    filter->plug(keytrack, FilterModule::kKeytrack);
    addSubmodule(filter.get());
    addProcessor(filter.get());
    processor_pool_["filter"].push_back(filter);
  }
}

void BlocksVoiceHandler::createDistortions() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto distortion = std::make_shared<DistortionModule>();
    distortion->plug(reset(), DistortionModule::kReset);
    addSubmodule(distortion.get());
    addProcessor(distortion.get());
    processor_pool_["drive"].push_back(distortion);
  }
}

void BlocksVoiceHandler::createChoruses() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto chorus = std::make_shared<ChorusModule>(beats_per_second_);
    addSubmodule(chorus.get());
    addProcessor(chorus.get());
    processor_pool_["chorus"].push_back(chorus);
  }
}


void BlocksVoiceHandler::createReverbs() {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto reverb = std::make_shared<ReverbModule>();
    addSubmodule(reverb.get());
    addProcessor(reverb.get());
    reverb->plug(reset(), ReverbModule::kReset);
    reverb->enable(false);
    processor_pool_["reverb"].push_back(reverb);
  }
}

void BlocksVoiceHandler::createDelays() {
  return;
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto delay = std::make_shared<DelayModule>(beats_per_second_);
    delay->plug(reset(), DelayModule::kReset);
    addSubmodule(delay.get());
    addProcessor(delay.get());
    delay->enable(false);
    processor_pool_["delay"].push_back(delay);
  }
}

std::shared_ptr<SynthModule> BlocksVoiceHandler::createProcessor(std::shared_ptr<model::Block> module) {
  std::shared_ptr<SynthModule> processor = processor_pool_[module->id.type][0];
  processor_pool_[module->id.type].erase(processor_pool_[module->id.type].begin());
  processor->enable(true);

  if (module->id.type == "osc") {
    processor->control_map_["on"]->set(1.0f);
    module->parameter_map_["wave"]->val = processor->control_map_["wave_frame"];
    module->parameter_map_["transpose"]->val = processor->control_map_["transpose"];
    module->parameter_map_["tune"]->val = processor->control_map_["tune"];
    module->parameter_map_["unison_voices"]->val = processor->control_map_["unison_voices"];
    module->parameter_map_["unison_detune"]->val = processor->control_map_["unison_detune"];
    module->parameter_map_["level"]->val = processor->control_map_["level"];
    module->parameter_map_["pan"]->val = processor->control_map_["pan"];
  } else if (module->id.type == "filter") {
    module->parameters_[0]->val = processor->control_map_["model"];
    module->parameters_[1]->val = processor->control_map_["style"];
    module->parameters_[2]->val = processor->control_map_["cutoff"];
    module->parameters_[3]->val = processor->control_map_["resonance"];
    module->parameters_[4]->val = processor->control_map_["blend"];
    module->parameters_[5]->val = processor->control_map_["mix"];
    processor->control_map_["on"]->set(1.0f);
  } else if (module->id.type == "reverb") {
    module->parameter_map_["mod rate"]->val = processor->control_map_["reverb_chorus_frequency"];
    module->parameter_map_["mod amount"]->val = processor->control_map_["reverb_chorus_amount"];
    module->parameter_map_["time"]->val = processor->control_map_["reverb_decay_time"];
    module->parameter_map_["mix"]->val = processor->control_map_["dry_wet"];
    module->parameter_map_["high cut"]->val = processor->control_map_["reverb_pre_high_cutoff"];
    module->parameter_map_["low cut"]->val = processor->control_map_["reverb_pre_low_cutoff"];
    module->parameter_map_["size"]->val = processor->control_map_["reverb_size"];
    module->parameter_map_["delay"]->val = processor->control_map_["reverb_delay"];
  } else if (module->id.type == "drive") {
    module->parameter_map_["type"]->val = processor->control_map_["type"];
    module->parameter_map_["drive"]->val = processor->control_map_["drive"];
    module->parameter_map_["mix"]->val = processor->control_map_["mix"];
  } else if (module->id.type == "chorus") {
    module->parameter_map_["voices"]->val = processor->control_map_["voices"];
    module->parameter_map_["depth"]->val = processor->control_map_["depth"];
    module->parameter_map_["frequency"]->val = processor->control_map_["frequency"];
    module->parameter_map_["feedback"]->val = processor->control_map_["feedback"];
    module->parameter_map_["mix"]->val = processor->control_map_["mix"];
    // voices_ = createBaseControl2({ .name = "chorus_voices" });
    // Output* free_frequency = createPolyModControl2({ .name = "frequency", .min = -6.0f, .max = 3.0f, .default_value = 6.0f,. value_scale = ValueScale::kExponential });
    // frequency_ = createTempoSyncSwitch("chorus", free_frequency->owner, beats_per_second_, false);
    // Output* feedback = createPolyModControl2({ .name = "feedback", .min = -0.95f, .max = 0.95f, .default_value = 0.4f });
    // wet_output_ = createPolyModControl2({ .name = "mix", .default_value = 0.5f });
    // Output* cutoff = createMonoModControl("chorus_cutoff");
    // Output* spread = createMonoModControl("chorus_spread");
    // mod_depth_ = createPolyModControl2({ .name = "mod_depth", .default_value = 0.5f });

    // delay_time_1_ = createMonoModControl("chorus_delay_1");
    // delay_time_2_ = createMonoModControl("chorus_delay_2");


    // add({ .name = "voices", .min = 1, .max = 4, .value_scale = ValueScale::kIndexed });
    // add({ .name = "depth", .default_value = 0.5f });
    // add({ .name = "frequency", .min = -6.0f, .max = 3.0f, .default_value = 6.0f, .value_scale = ValueScale::kExponential });
    // add({ .name = "feedback", .min = -0.95f, .max = 0.95f, .default_value = 0.4f });
    // add({ .name = "mix", .default_value = 0.5f });
  }

  auto index = module->index;
  processor_matrix_[index.column][index.row] = processor;
  active_processor_map_[module->name] = processor;
  active_processors_.push_back(processor);
  // processors_[module->id.type].push_back(processor);
  processor->setModule(module);
  return processor;
}

void BlocksVoiceHandler::createOscillators() {
  std::string type = "osc";
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto osc = std::make_shared<OscillatorModule>();

    osc->plug(reset(), OscillatorModule::kReset);
    osc->plug(retrigger(), OscillatorModule::kRetrigger);
    osc->plug(bent_midi_, OscillatorModule::kMidi);
    osc->plug(active_mask(), OscillatorModule::kActiveVoices);
    addSubmodule(osc.get());
    addProcessor(osc.get());

    processor_pool_[type].push_back(osc);
    oscillators_.push_back(osc);
  }
}

void BlocksVoiceHandler::clear() {
  for (auto processor : active_processors_) {
    processor_pool_[processor->module_->id.type].push_back(processor);
    processor->enable(false);
  }

  active_processors_.clear();
  active_processor_map_.clear();

  active_modulators_.clear();
  active_modulators_map_.clear(); // expect for default env 

  unplugAll();
}

void BlocksVoiceHandler::createModulators() {
  for (int i = 0; i < kNumLfos; ++i) {
    lfo_sources_[i].setLoop(false);
    lfo_sources_[i].initTriangle();
    std::string prefix = std::string("lfo");
    auto lfo = std::make_shared<LfoModule>(prefix, &lfo_sources_[i], beats_per_second_);
    addSubmodule(lfo.get());
    addProcessor(lfo.get());
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
    createEnvelope();
  }

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

std::shared_ptr<EnvelopeModule> BlocksVoiceHandler::createEnvelope(bool audio_rate) {
  auto envelope = std::make_shared<EnvelopeModule>(audio_rate);
  envelope->plug(retrigger(), EnvelopeModule::kTrigger);
  addSubmodule(envelope.get());
  addProcessor(envelope.get());
  envelopes_.push_back(envelope);
  processor_pool_["envelope"].push_back(envelope);

  createStatusOutput("env", envelope->output(EnvelopeModule::kValue));
  createStatusOutput("env_phase", envelope->output(EnvelopeModule::kPhase));
  data_->mod_sources["env"] = envelope->output();
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

  Value* val = new Value(0.5f);
  addProcessor(val);

  amplitude_ = new Square();
  amplitude_->plug(val);
  addProcessor(amplitude_);
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
        // utils::print_mask(last_active_voice_mask_, "mask  at handler", processor->output());
        buffer[0] = masked_value;// + utils::swapVoices(masked_value);
        // utils::print(buffer[0], "value at handler", this);
      } else {
        for (int i = 0; i < num_samples; ++i) {
          poly_float masked_value = buffer[i] & last_active_voice_mask_;
          buffer[i] = masked_value;// + utils::swapVoices(masked_value);
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
  for (auto envelope : envelopes_) {
    if (output->owner == envelope.get())
      std::cout << "yep" << std::endl;
    return false;
  }

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

void BlocksVoiceHandler::setOSCAmplitudeEnvelope(std::shared_ptr<model::Module> adsr, std::shared_ptr<model::Module> target) {
  auto osc = dynamic_cast<OscillatorModule*>(active_processor_map_[target->name].get());
  auto env = dynamic_cast<EnvelopeModule*>(active_modulators_map_[adsr->name].get());
  osc->switchLevelEnvelope(env);
  // osc->amplitude_envelope_->followModule(adsr);
}

void BlocksVoiceHandler::resetOSCAmplitudeEnvelope(std::shared_ptr<model::Module> target) {
  auto osc = dynamic_cast<OscillatorModule*>(active_processor_map_[target->name].get());
  osc->resetAmpADSR();
}

} // namespace vital