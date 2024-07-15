#include "scale.hpp"

#include <iostream>

Scale::Scale(Note tonic, ScaleType typ):
    notes_()
{
    setScale(tonic,typ);
}

Scale::Scale():
    Scale(Note::C, ScaleType::MAJOR)
{}

void Scale::setScale(Note tonic, ScaleType typ){
    tonic_ = tonic ;
    type_ = typ ;
    populateScale();
}

const std::pair<Note*, size_t> Scale::getScale(){
    return std::make_pair(notes_.data(), notes_.size());

}

void Scale::populateScale(){
    // if scale isn't set, create dummy array
    if (tonic_ == Note::NULL_NOTE){
        for (int i = 0; i < notes_.size(); ++i){
            notes_[i] = Note::NULL_NOTE ;
        }
        return ;
    }

    // otherwise, populate scale based off the scale intervals
    auto intervals = getScaleIntervals(type_);
    size_t i = 0 ;
    
    for (; i < intervals.second; ++i ){
        notes_[i] = static_cast<Note>(static_cast<uint8_t>(tonic_) + intervals.first[i]);
    }

    // pad the rest of the array with null notes
    for (; i < MAX_NOTES; ++i) notes_[i] = Note::NULL_NOTE ;
    
}

uint8_t Scale::getNearestScaleMidiNote(uint8_t v){
    std::cout << "[Scale::getNearestScaleMidiNote] input value: " << static_cast<int>(v) << std::endl ;
    uint8_t octave = getMidiOctave(v) ;
    uint8_t uNote = static_cast<uint8_t>(getMidiNoteName(v));
    for (Note x : notes_ ){
        if ( uNote <= static_cast<uint8_t>(x) ){
            return getMidiValue(x,octave);
        }
    }

    return getMidiValue(notes_[notes_.size() - 1], octave);
}

uint8_t Scale::getMidiOctave(uint8_t v){
    return ( v / 12 ) - 1 ;
}

Note Scale::getMidiNoteName(uint8_t v){
    return static_cast<Note>( v % 12 ) ;
}

uint8_t Scale::getMidiValue(Note n, uint8_t octave){
    return static_cast<uint8_t>(n) + (12 * (octave + 1) );
}