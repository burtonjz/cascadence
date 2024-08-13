#ifndef __CASCADENCE_HPP_
#define __CASCADENCE_HPP_

#include "urids.hpp"
#include "Sequence.hpp"
#include "MidiController.hpp"
#include "ParameterController.hpp"

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/atom/forge.h>

#include <cstdint>

class Cascadence {
private:
    // LV2_Log_Logger logger_ ;
    LV2_URID_Map* uridMap_ ;
    Urids urids_ ;

    const double sampleRate_ ;
    const double samplePeriod_ ;

    LV2_Atom_Forge forge_ ;    
    const LV2_Feature *const *features_ ;
    
    uint32_t bpm_ ;

    Sequence Sequence_ ;
    MidiController MidiController_ ;
    ParameterController ParamController_ ;

public:
    /**
     * @brief Construct a new Cascadence object
     * 
     * @param sampleRate host sample rate
     * @param features LV2 features array
     */
    Cascadence(const double sampleRate, const LV2_Feature *const *features);

    ParameterController* getParameterController();
    
    // API FUNCTIONS
    void connectPort(const uint32_t port, void* data);
    void activate();
    void run(const uint32_t sampleCount);
    void deactivate();

private:
    /**
     * @brief ticks the application forward
     * 
     */
    void tick();

    /**
     * @brief run sequencing logic in the run loop
     * 
     * @param start start frame index to sequence
     * @param end end frame index to sequence
     */
    void sequence(const uint32_t start, const uint32_t end);

    /**
     * @brief verifies if a specified midi value is in the bounds 0-127
     * 
     * @param midiVal the midi value
     * @param d delta to add to midi value
     */
    bool isMidiInBounds(uint8_t midiVal, int d = 0);
};


#endif // __CASCADENCE_HPP_