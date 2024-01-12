#include "module_new.h"
#include "vital/synthesis/effects/delay.h"
#include "vital/synth_strings.h"

namespace model {
class DelayModule: public Block {
public:
  DelayModule(int number): Block("delay", number) {
    
// delay: [
// { name: 'feedback', min : -1, default_value : 0.5 },
// { name: 'filter_cutoff', min : 8, max : 136, default_value : 60 },
// { name: 'filter_spread', default_value : 1 },
// { name: 'frequency', min : -2, max : 9, value_scale : 6, default_value : 2 },
// { name: 'on', value_scale : 0, default_value : 0 },
// { name: 'style', max : 3, value_scale : 0, default_value : 0 },
// { name: 'sync', max : 3, value_scale : 0, default_value : 1 },
// { name: 'tempo', min : 4, max : 12, value_scale : 0, default_value : 9 },
// { name: 'aux_frequency', min : -2, max : 9, value_scale : 6, default_value : 2 },
// { name: 'aux_sync', max : 3, value_scale : 0, default_value : 1 },
// { name: 'aux_tempo', min : 4, max : 12, value_scale : 0, default_value : 9 }
// ],
  }
};
}