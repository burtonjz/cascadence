#ifndef __URIDS_HPP_
#define __URIDS_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/buf-size/buf-size.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>

#define CASCADENCE_URI "https://github.com/burtonjz/cascadence"

struct Urids {
    LV2_URID midiEvent ;
    LV2_URID bufSize_maxBlockSize ;
    LV2_URID atomPath;
    LV2_URID atomResource;
    LV2_URID atomSequence;
    LV2_URID atomURID;
    LV2_URID atomEventTransfer;
    LV2_URID patchSet;
    LV2_URID patchProperty;
    LV2_URID patchValue;


    void initialize(LV2_URID_Map* m){
        midiEvent = m->map(m->handle, LV2_MIDI__MidiEvent);
        bufSize_maxBlockSize = m->map(m->handle, LV2_BUF_SIZE__maxBlockLength);
        atomPath = m->map(m->handle, LV2_ATOM__Path);
        atomResource = m->map(m->handle, LV2_ATOM__Resource);
        atomSequence = m->map(m->handle, LV2_ATOM__Sequence);
        atomURID = m->map(m->handle, LV2_ATOM__URID);
        atomEventTransfer = m->map(m->handle, LV2_ATOM__eventTransfer);
        patchSet = m->map(m->handle, LV2_PATCH__Set);
        patchProperty = m->map(m->handle, LV2_PATCH__property);
        patchValue = m->map(m->handle, LV2_PATCH__value);
    }
};

#endif // __URIDS_HPP_