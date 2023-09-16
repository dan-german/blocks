#include "NoteLogger.h"

void NoteLogger::log(Array<MPENote> notes) {
  Array<int> stoppedNotes;
  Array<int> startedNotes;

  for (auto it = noteIDs.begin(); it != noteIDs.end();) {
    if (std::find_if(notes.begin(), notes.end(),
      [it](const MPENote& newNote) { return *it == newNote.noteID; }) == notes.end()) {
      stoppedNotes.add(*it);
      it = noteIDs.erase(it);
    } else {
      ++it;
    }
  }

  for (const auto& newNote : notes)
    if (noteIDs.insert(newNote.noteID).second)
      startedNotes.add(newNote.noteID);

  if (startedNotes.size() > 0) listener->notesStarted(startedNotes);
  if (stoppedNotes.size() > 0) listener->notesEnded(stoppedNotes);
}