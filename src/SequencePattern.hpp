#ifndef __SEQUENCE_PATTERN_HPP_
#define __SEQUENCE_PATTERN_HPP_

#include "config.hpp"
#include <algorithm>
#include <initializer_list>
#include <iostream>

struct SequencePattern {
private:
    size_t size_ ;

public: 
    int notes[CONFIG_MAX_SEQUENCE_SIZE] ; // in scale positions
    float start[CONFIG_MAX_SEQUENCE_SIZE] ; // in beats
    float end[CONFIG_MAX_SEQUENCE_SIZE] ; // in beats
    float duration ; // sequence length in beats

    void setSize(int s){
        if ( size_ > CONFIG_MAX_SEQUENCE_SIZE ){
            std::cerr << "[SequencePattern::setSize] ERROR: size " << s << " is larger than the configured maximum. " << std::endl ;
            return ;
        }
        size_ = s ;
    }

    size_t size() const {
        return size_ ;
    }

    template<typename T>
    void setArray(T* array, std::initializer_list<T> values) {
        if ( values.size() != size_ ){
            std::cerr << "[SequencePattern::setArray] ERROR: initializer list is of wrong size" << std::endl ;
            return ;
        }
        std::copy(values.begin(), values.end(), array);
    }
};

#endif // __SEQUENCE_PATTERN_HPP_