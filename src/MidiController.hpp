#ifndef __MIDI_WRITER_HPP_
#define __MIDI_WRITER_HPP_

#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <cstdint>
#include <array>

#include "config.hpp"
#include "midiNoteEvent.hpp"
#include "Sequence.hpp"




class MidiController {
private:
    Sequence* sequence_ptr_ ;
    const LV2_Atom_Sequence* input_ ;
    LV2_Atom_Sequence* output_ ;
    uint32_t capacity_ ;
    std::array<uint8_t, CONFIG_MAX_SEQUENCE_SIZE> activeNotes_ ;
public:
    MidiController();

    // set pointer to sequence object
    void setSequence(Sequence* ptr);

    /**
     * @brief Set the midi output pointer
     * 
     * @param m supplied midi input pointer (From LV2 Host)
     */
    void setInput(LV2_Atom_Sequence* m);

    /**
     * @brief get pointer to midi input buffer
     * 
     * @return const LV2_Atom_Sequence* input
     */
    const LV2_Atom_Sequence* getInput() const ;

    /**
     * @brief Set the midi output pointer
     * 
     * @param m supplied midi output pointer (From LV2 Host)
     */
    void setOutput(LV2_Atom_Sequence* m);

    /**
     * @brief Perform all necessary tasks when receiving a new buffer from the Host
     * 
     */
    void prepareBuffer();

    /**
     * @brief handle incoming midi events
     * 
     * @param ev LV2 Atom Event pointer
     */
    void processInput(LV2_Atom_Event* ev);

    /**
     * @brief run loop through incoming midi
     * 
     */
    void runLoop();

    /**
     * @brief Append a new midi event to the output buffer
     * 
     * @param m Midi note event
     */
    void append(MidiNoteEvent m);

    /**
     * @brief checks active notes for the specified midi value
     * 
     * @param midiVal midi value (0-127)
     */
    bool isMidiOn(uint8_t midiVal);

private:
    void updateActive(MidiNoteEvent m);

};

#endif // __MIDI_WRITER_HPP_
