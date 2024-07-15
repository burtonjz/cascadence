#ifndef __SCALE_TYPE_HPP_
#define __SCALE_TYPE_HPP_

#include <utility>
#include <cstdint>
#include <cstddef>

enum class ScaleType {
    MAJOR,
    NATURAL_MINOR,
    MELODIC_MINOR,
    HARMONIC_MINOR,
    CHROMATIC
};

/**
 * @brief Get the intervals array for a specified scale
 * 
 * @param stype Scale Type
 * @return std::pair<uint8_t*, size_t> interval array
 */
std::pair<uint8_t*, size_t> getScaleIntervals(ScaleType stype);

#endif // __SCALE_TYPE_HPP_