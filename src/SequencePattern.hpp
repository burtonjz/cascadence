#ifndef __SEQUENCE_PATTERN_HPP_
#define __SEQUENCE_PATTERN_HPP_

#include "config.hpp"
#include <cstdint>

struct SequencePattern {
    int notes[CONFIG_MAX_SEQUENCE_SIZE] ; // in scale positions
    float durations[CONFIG_MAX_SEQUENCE_SIZE] ; // in beats
    int length ;
};

#endif // __SEQUENCE_PATTERN_HPP_