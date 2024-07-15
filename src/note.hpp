#ifndef __TONIC_HPP_
#define __TONIC_HPP_

#include <cstdint>

enum class Note : uint8_t {
    C = 0,
    CSHARP = 1,
    DFLAT = 1,
    D = 2,
    DSHARP = 3,
    EFLAT = 3,
    E = 4,
    F = 5,
    FSHARP = 6,
    GFLAT = 6,
    G = 7,
    GSHARP = 8,
    AFLAT = 8,
    A = 9,
    ASHARP = 10,
    BFLAT = 10,
    B = 11,
    NULL_NOTE = 12 
};

#endif // __TONIC_HPP_