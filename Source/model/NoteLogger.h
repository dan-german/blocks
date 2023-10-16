#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

using namespace juce;

class NoteLogger {
private:
  std::unordered_set<int> noteIDs;
public:
  struct Listener;
  Listener* listener;
  void log(Array<MPENote> newNotes);
};

struct NoteLogger::Listener {
  virtual void notesStarted(Array<int> noteIDs) = 0;
  virtual void notesEnded(Array<int> noteIDs) = 0;
};