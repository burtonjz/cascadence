#ifndef __CASCADENCE_HPP_
#define __CASCADENCE_HPP_

#include "urids.hpp"
#include "midiNoteEvent.hpp"

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>

#include <cstdint>

class Cascadence {
private:
    // LV2_Log_Logger logger_ ;

    const LV2_Atom_Sequence* midiIn_ ;
    LV2_Atom_Sequence* midiOut_ ;

    LV2_URID_Map* uridMap_ ;
    Urids urids_ ;

    double sampleRate_ ;

public:
    /**
     * @brief Construct a new Cascadence object
     * 
     * @param sampleRate host sample rate
     * @param features LV2 features array
     */
    Cascadence(const double sampleRate, const LV2_Feature *const *features);

    void connectPort(const uint32_t port, void* data);
    void activate();
    void run(const uint32_t sampleCount);
    void deactivate();

private:
    /**
     * @brief append a midi event to the stream
     * 
     * @param mEvent reference midi note event
     * @param dSemitones number of semitones to shift the note (default = 0)
     * @param dFrames number of sample frames to shift the note (default = 0)
     * @param dVelocity amount to shift midi velocity value (default = 0)
     */
    void appendMidi(MidiNoteEvent* mEvent, int dSemitones = 0, int dFrames = 0, int dVelocity = 0);

    /**
     * @brief verifies if a specified midi value is in the bounds 0-127
     * 
     * @param midiVal the midi value
     * @param d delta to add to midi value
     */
    bool isMidiInBounds(uint8_t midiVal, int d = 0);
};


#endif // __CASCADENCE_HPP_