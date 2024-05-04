#include "slider_container.h"

namespace gui { 
  class Inspector: public juce::Component { 
    private:    
      SliderContainer slider_container_;

      Inspector() {
        addAndMakeVisible(slider_container_);
      }

      void resized() override {
        slider_container_.setBounds(getLocalBounds());
      }
  }
}