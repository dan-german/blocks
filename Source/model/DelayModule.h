/*
  ==============================================================================

    DelayModule.h
    Created: 8 Jun 2021 8:32:27pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/Block.h"
#include "model/ModelConstants.h"

namespace Model {
struct DelayModule: public Block {
  enum Parameters { pFeedback, pSync, pTime, pMix };
  inline static int count = 0;
  DelayModule(int number);
  ~DelayModule() {}
};
}