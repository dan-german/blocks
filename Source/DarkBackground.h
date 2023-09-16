#include "EasingAnimator.h"
#include "juce_gui_basics/juce_gui_basics.h"
using namespace juce;

class DarkBackground: public Component {
private:
  const float Alpha = 0.2f;
  EasingAnimator animator;
  Colour colour;
protected:
  void resized() override;
  void paint(juce::Graphics& g) override;
  void mouseUp(const MouseEvent& event) override;
public:
  std::function<void(Component*)> onClick;
  Path path;
  DrawablePath darkener;
  void setVisible(bool shouldBeVisible) override;
  DarkBackground();
  ~DarkBackground() override {};
  float enabledAlpha = 0.35f;
};