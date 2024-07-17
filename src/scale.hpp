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
    std::array<Note,MAX_NOTES> notes_ ;
    std::array<uint8_t,MAX_NOTES> uNotes_ ;
    size_t notesSize_ ;

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
     * @brief Get the Scale array (as Notes)
     * 
     * @return const std::pair<size_t,Note*> 
     */
    const std::pair<Note*,size_t> getScale() ;

    /**
     * @brief Get the Scale array (as Midi Values)
     * 
     * @return const std::pair<size_t,Note*> 
     */
    const std::pair<uint8_t*,size_t> getUScale() ;

    /**
     * @brief get closest scale value. Rounds down.
     * 
     * @param v reference midi value
     * @param offset scale position offset
     * @return uint8_t midi value
     */
    uint8_t getNearestScaleMidiNote(uint8_t v, int offset = 0) ;

private:
    /**
     * @brief populates the notes array with the currently defined scale
     */
    void populateScale();

    /**
     * @brief Get the midi octave. *Midi note 0 corresponds with octave -1
     * 
     * @param v midi value
     * @return int octave number
     */
    int getMidiOctave(uint8_t v);

    /**
     * @brief get the Note corresponding to a midi note
     * 
     * @param v midi value
     * @return Note
     */
    Note getMidiNoteName(uint8_t v);
    
    /**
     * @brief Get the Midi Value from the note/octave
     * 
     * @param n note
     * @param octave midi octave (starts at -1) 
     * @return uint8_t midi value
     */
    uint8_t getMidiValue(Note n, int octave);

    /**
     * @brief true modulo (needed for negative numbers
     * 
     */
    int modulo(int a, int b);

};


#endif // __SCALE_HPP_
