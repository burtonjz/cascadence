#ifndef __SEQUENCE_PATTERN_HPP_
#define __SEQUENCE_PATTERN_HPP_

#include "config.hpp"

// contains configuration information for a given note in a sequence
struct SequenceNote {
    uint8_t note ; // in scale position
    float start ; // in beats
    float end ; // in beats

};

class SequencePattern {
private:
    size_t size_ ;
    float duration_ ; // sequence length in beats
    SequenceNote notes_[CONFIG_MAX_SEQUENCE_SIZE] ;

public:
    /**
     * @brief SequencePattern default constructor
     *
     */
    SequencePattern();

    size_t size() const ;

    float getDuration() const ;

    /**
     * @brief retrieve the sequence note
     *
     * @param i note index
     */
    const SequenceNote getSequenceNote(size_t i) const ;

    /**
     * @brief append a new sequence note to the sequence
     *
     * @param note note scale index
     * @param start start beat
     * @param end end beat
     */
    void append(uint8_t note, float start, float end );

    /**
     * @brief remove a sequence note
     *
     * @param index element index
     */
    void erase(size_t index);
    /**
     * @brief reset the sequence to empty
     *
     */
    void reset();
};

#endif // __SEQUENCE_PATTERN_HPP_
