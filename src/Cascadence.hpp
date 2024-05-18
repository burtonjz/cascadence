#ifndef __CASCADENCE_HPP_
#define __CASCADENCE_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>

#define CASCADENCE_URI "https://github.com/burtonjz/cascadence"

class Cascadence {
private:
    const LV2_Atom_Sequence* midiIn_ ;
    const LV2_Atom_Sequence* midiOut_ ;
    LV2_URID_Map* uridMap_ ;

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
    void run(const uint32_t sample_count);
    void deactivate();

};


#endif // __CASCADENCE_HPP_