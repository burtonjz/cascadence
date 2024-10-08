#ifndef __URIDS_HPP_
#define __URIDS_HPP_

#include <lv2/lv2plug.in/ns/lv2core/lv2.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/ext/options/options.h>
#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <lv2/lv2plug.in/ns/ext/patch/patch.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>

// define plugin URIs
#define CASCADENCE_URI "https://github.com/burtonjz/cascadence"
#define CASCADENCE__bypass     CASCADENCE_URI "#bypass"
#define CASCADENCE__bpm        CASCADENCE_URI "#bpm"
#define CASCADENCE__scaleType  CASCADENCE_URI "#scaleType"
#define CASCADENCE__scaleTonic CASCADENCE_URI "#scaleTonic"

struct Urids {
    LV2_URID plugin ;
    LV2_URID midiEvent ;
    // ATOM
    LV2_URID atomPath ;
    LV2_URID atomResource ;
    LV2_URID atomSequence ;
    LV2_URID atomURID ;
    LV2_URID atomEventTransfer ;
    // PATCH
    LV2_URID patchGet ;
    LV2_URID patchSet ;
    LV2_URID patchPut ;
    LV2_URID patchBody ;
    LV2_URID patchSubject ;
    LV2_URID patchProperty ;
    LV2_URID patchValue ;
    // PLUGIN
    LV2_URID plugBypass ;
    LV2_URID plugBpm    ;
    LV2_URID plugScaleType ;
    LV2_URID plugScaleTonic ;


    void initialize(LV2_URID_Map* m){
        plugin            = m->map(m->handle, CASCADENCE_URI);
        midiEvent         = m->map(m->handle, LV2_MIDI__MidiEvent);
        atomPath          = m->map(m->handle, LV2_ATOM__Path);
        atomResource      = m->map(m->handle, LV2_ATOM__Resource);
        atomSequence      = m->map(m->handle, LV2_ATOM__Sequence);
        atomURID          = m->map(m->handle, LV2_ATOM__URID);
        atomEventTransfer = m->map(m->handle, LV2_ATOM__eventTransfer);
        patchGet          = m->map(m->handle, LV2_PATCH__Get);
        patchSet          = m->map(m->handle, LV2_PATCH__Set);
        patchPut          = m->map(m->handle, LV2_PATCH__Put);
        patchBody         = m->map(m->handle, LV2_PATCH__body);
        patchSubject      = m->map(m->handle, LV2_PATCH__subject);
        patchProperty     = m->map(m->handle, LV2_PATCH__property);
        patchValue        = m->map(m->handle, LV2_PATCH__value);
        plugBypass        = m->map(m->handle, CASCADENCE__bypass);
        plugBpm           = m->map(m->handle, CASCADENCE__bpm);
        plugScaleType     = m->map(m->handle, CASCADENCE__scaleType);
        plugScaleTonic    = m->map(m->handle, CASCADENCE__scaleTonic);
    }
};

#endif // __URIDS_HPP_
