#ifndef __SCALE_TYPE_HPP_
#define __SCALE_TYPE_HPP_

#include <utility>
#include <cstdint>
#include <cstddef>
#include <string>

enum class ScaleType {
    MAJOR,
    NATURAL_MINOR,
    MELODIC_MINOR,
    HARMONIC_MINOR,
    CHROMATIC,
    N_SCALES
};

/**
 * @brief Get the intervals array for a specified scale
 *
 * @param stype Scale Type
 * @return std::pair<uint8_t*, size_t> interval array
 */
std::pair<uint8_t*, size_t> getScaleIntervals(ScaleType stype);


/**
 * @brief Get the corresponding string for a scale type
 *
 * @param stype scale type enum
 */
std::string getScaleString(ScaleType stype);

#endif // __SCALE_TYPE_HPP_
