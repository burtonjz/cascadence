#ifndef SEQUENCE_HPP_
#define SEQUENCE_HPP_

#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <array>
#include <utility>

#include "config.hpp"
#include "SequencePattern.hpp"
#include "scale.hpp"
#include "midiNoteEvent.hpp"

// forward declarations
class MidiController ;

/**
 * @brief define and implement a midi sequence
 * 
 */
class Sequence {
private:
    const double* sampleRate_ ;
    MidiController* controller_ptr_ ;
    SequencePattern pattern_ ;
    std::array<int, CONFIG_MAX_SEQUENCE_SIZE> startFrames ;
    std::array<int, CONFIG_MAX_SEQUENCE_SIZE> endFrames ;
    bool isPressed_ ;

    Scale Scale_ ;
    MidiNoteEvent Root_ ;
    int frame_ ;
    int bpm_ ;

public:
    /**
     * @brief default constructor
     * 
     */
    Sequence(const double* sampleRate);

    Sequence(const double* sampleRate, Scale scale, int bpm, SequencePattern pattern);

    /**
     * @brief set midi controller pointer
     * 
     * @param ptr pointer to MidiController object
     */
    void setMidiController(MidiController* ptr);

    /**
     * @brief define sequence pattern
     * 
     * @param pattern sequence pattern
     */
    void setPattern(SequencePattern pattern);

    /**
     * @brief set the midi event status message type
     * 
     * @param midiStatus midi status value
     */
    void setMidiStatus(LV2_Midi_Message_Type midiStatus);

    /**
     * @brief updates the sequence based off the target message
     * 
     * @param midiMsg midi message
     */
    void setRootNote(MidiNoteEvent m);

    /**
     * @brief perform sequencing for all midi note events
     * 
     */
    void sequenceMidiNoteEvents();

    /**
     * @brief tick sequence object
     * 
     */
    void tick(); // frame_ += 1 ;

private:
    /**
     * @brief calculate start and end frames given current configuration
     * 
     */
    void calculateFrameTiming();
};

#endif // SEQUENCE_HPP_