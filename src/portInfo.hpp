#ifndef __PORT_INFO_HPP_
#define __PORT_INFO_HPP_

#include <cstdint>

enum PortInfo : uint32_t {
    PORT_MIDI_OUT,
    PORT_MIDI_IN,
    PORT_N_PORTS
};

#endif // __PORT_INFO_HPP_