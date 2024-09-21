#include "SequencePattern.hpp"
#include "config.hpp"
#include <iostream>


SequencePattern::SequencePattern():
    size_(0),
    duration_(0)
{}

size_t SequencePattern::size() const {
    return size_ ;
}

float SequencePattern::getDuration() const {
    return duration_ ;
}

const SequenceNote SequencePattern::getSequenceNote(size_t index) const {
    if ( index > size_ ){
        std::cerr << "[SequencePattern::getSequenceNote] ERROR: index out of range" << std::endl ;
        throw ;
    }

    return notes_[index] ;

}

void SequencePattern::append(uint8_t note, float start, float end ){
    if ( size_ >= CONFIG_MAX_SEQUENCE_SIZE ){
        std::cerr << "[SequencePattern::append] ERROR: sequence is full, cannot append a new sequence note" << std::endl ;
    }
    if ( start > end ){
        std::cerr << "[SequencePattern::append] ERROR: start beat "
            << start << " is larger than the end beat " << end << std::endl ;
        return ;
    };

    notes_[size_].note = note ;
    notes_[size_].start = start ;
    notes_[size_].end = end ;

    if ( end > duration_ ) duration_ = end ;
    ++size_ ;
}

void SequencePattern::erase(size_t index){
    if (index >= size_){
            std::cerr << "[SequencePattern::erase] ERROR: index out of range." << std::endl ;
            return ;
    }
    for (size_t i = index; i < size_ - 1; ++i){
        notes_[i] = notes_[i + 1] ;
    }

    --size_ ;
}

void SequencePattern::reset(){
    size_ = 0 ;
    duration_ = 0 ;
}
