#ifndef __SCALE_HPP_
#define __SCALE_HPP_

#include "note.hpp"
#include "scaleType.hpp"

#include <array>
#include <cstddef>

constexpr size_t MAX_NOTES = 12 ; // max notes in chromatic scale

class Scale {
private:
    Note tonic_ ;
    ScaleType type_ ;
    std::array<uint8_t,MAX_NOTES> scaleNotes_ ;
    size_t scaleLength_ ;

public:
    /**
     * @brief Constructor for Key Class
     *
     * Create a new Key object, contains the valid midi notes for a given key
     * @param tonic the tonic note
     * @param typ the scale type
     */
    Scale(Note tonic, ScaleType typ);

    /**
     * @brief Default constructor. Defaults to a C Major scale.
     */
    Scale();

    /**
     * @brief repopulate the scale with new inputs
     *
     * @param tonic the tonic note
     * @param typ the scale type
     */
    void setScale(Note tonic, ScaleType typ);

    /**
     * @brief Get the Scale array (as Midi Values)
     *
     * @return const std::pair<size_t,Note*>
     */
    const std::pair<uint8_t*,size_t> getScale() ;

    /**
     * @brief get closest scale value. Rounds down.
     *
     * @param v reference midi value
     * @param offset scale position offset
     * @return uint8_t midi value
     */
    uint8_t getNearestScaleMidiNote(uint8_t v, int offset = 0) ;

    /**
     * @brief update the scale type of the scale
     *
     */
    void setScaleType(ScaleType typ);

    /**
     * @brief update the tonic of the scale
     *
     */
    void setTonic(Note tonic);

private:
    /**
     * @brief populates the notes array with the currently defined scale
     */
    void populateScale();

    /**
     * @brief true modulo (needed for negative numbers
     *
     */
    int modulo(int a, int b);

};


#endif // __SCALE_HPP_
