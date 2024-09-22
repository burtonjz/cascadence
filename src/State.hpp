#ifndef __STATE_HPP_
#define __STATE_HPP_

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>
#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <cstring>

struct PluginState {
    LV2_Atom_Bool bypass ;
} ;

struct StateMapItem {
    const char* uri ;
    LV2_URID urid ;
    LV2_Atom* value ;

    bool operator<(const StateMapItem& other) const {
        return urid < other.urid ;
    }

    bool operator==(const StateMapItem& other) const {
        return urid == other.urid &&
            std::strcmp(uri,other.uri) == 0 &&
            value == other.value ;
    }
} ;

#endif // __STATE_HPP_
