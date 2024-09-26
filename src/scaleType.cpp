#include "scaleType.hpp"
#include <array>

std::pair<uint8_t*, size_t> getScaleIntervals(ScaleType stype){
    static std::array<uint8_t, 12> n ;
    switch(stype){
    case ScaleType::MAJOR:
        n = {0,2,4,5,7,9,11};
        return std::make_pair(n.data(),7);
    case ScaleType::NATURAL_MINOR:
        n = {0,2,3,5,7,8,10};
        return std::make_pair(n.data(),7);
    case ScaleType::MELODIC_MINOR:
        n = {0,2,3,5,7,9,11};
        return std::make_pair(n.data(),7);
    case ScaleType::HARMONIC_MINOR:
        n = {0,2,3,5,7,8,11};
        return std::make_pair(n.data(),7);
    case ScaleType::CHROMATIC:
        n = {0,1,2,3,4,5,6,7,8,9,10,11};
        return std::make_pair(n.data(), n.size()) ;
    default:
        return std::make_pair(n.data(),0) ;
    }
}

constexpr std::array<const char*, static_cast<int>(ScaleType::N_SCALES)> scaleNames = {
    "Major",
    "Natural Minor",
    "Melodic Minor",
    "Harmonic Minor",
    "Chromatic"
};

std::string getScaleString(ScaleType stype){
    size_t i = static_cast<size_t>(stype);
    if ( i < static_cast<size_t>(ScaleType::N_SCALES)){
        return scaleNames[i] ;
    }
    return "Unknown Scale" ;
};
