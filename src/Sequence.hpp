#ifndef SEQUENCE_HPP_
#define SEQUENCE_HPP_

#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <array>

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
    float duration_ ;

    Scale Scale_ ;
    MidiNoteEvent root_ ;
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
     * @brief Get a pointer to the Scale object
     *
     * @return Scale*
     */
    Scale* getScale();

    /**
     * @brief Set the beats per minute of the sequence
     *
     * @param bpm beats per minute
     */
    void setBpm(int bpm) ;

    /**
     * @brief toggle the sequence on or off
     *
     * @param status bool
     */
    void setStatus(bool status);

    /**
     * @brief getter for root note
     */
    const MidiNoteEvent getRootNote() const ;

    /**
     * @brief perform sequencing for all midi note events
     *
     */
    void sequenceMidiNoteEvents();

    /**
     * @brief Handle incoming Midi Note Event
     * @param m Midi Note Event
     */
    void handleMidiNoteEvent(const MidiNoteEvent m);

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
