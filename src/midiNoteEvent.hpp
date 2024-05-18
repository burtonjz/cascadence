#ifndef __MIDI_NOTE_EVENT_HPP_
#define __MIDI_NOTE_EVENT_HPP_

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <cstdint>

struct MidiNoteEvent {
    LV2_Atom_Event event ;
    uint8_t msg[3];
};

#endif // __MIDI_NOTE_EVENT_HPP_