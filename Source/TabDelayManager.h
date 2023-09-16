#include "juce_audio_basics/juce_audio_basics.h"

class Tap {
public:
  MPENote note;
  int samplesLeftToTrigger;
  bool active;
  int originalNoteDuration = 0;
  bool originalNoteStopped = false;
};

class TabDelayManager {
public:
  static const int maxTaps = 1000; // TODO: make this dynamic somehow
  Tap taps[maxTaps];
  int tapCount = 0;
  std::function<void(MPENote)> onTapStarted;
  std::function<void(MPENote)> onTapEnded;
  // queue a tap 
  void addTap(MPENote note, int delayInSamples) {
    if (tapCount >= maxTaps) return;
    taps[tapCount++] = Tap { note, delayInSamples, false };
  }

  void setStopped(MPENote note) {
    for (int i = 0; i < tapCount; i++) {
      bool sameChannel = taps[i].note.midiChannel == note.midiChannel;
      bool sameNote = taps[i].note.initialNote == note.initialNote;
      if (sameNote && sameChannel) {
        taps[i].originalNoteStopped = true;
      }
    }
  }

  void process(int numSamples) {
    if (tapCount == 0) return;

    for (int i = 0; i < maxTaps; i++) {
      Tap& tap = taps[i];

      if (!tap.originalNoteStopped)
        tap.originalNoteDuration += numSamples;

      if (tap.samplesLeftToTrigger > 0) {
        tap.samplesLeftToTrigger -= numSamples;
        if (tap.samplesLeftToTrigger <= 0) {
          tap.active = true;
          onTapStarted(tap.note);
        }
      } else if (tap.active && tap.originalNoteStopped) {
        tap.originalNoteDuration -= numSamples;
        if (tap.originalNoteDuration <= 0) {
          tap.active = false;
          onTapEnded(tap.note);
          tapCount--;
        }
      }
    }
  }
};