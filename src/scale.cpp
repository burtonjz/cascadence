#include "scale.hpp"
#include "config.hpp"

#include <cmath>

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
    return std::make_pair(notes_.data(), notesSize_);

}

void Scale::populateScale(){
    // if scale isn't set, create dummy array
    if (tonic_ == Note::NULL_NOTE){
        for (int i = 0; i < notesSize_; ++i){
            notes_[i] = Note::NULL_NOTE ;
        }
        return ;
    }

    // otherwise, populate scale based off the scale intervals
    auto intervals = getScaleIntervals(type_);
    notesSize_ = intervals.second ;
    size_t i = 0 ;
    
    for (; i < notesSize_ ; ++i ){
        notes_[i] = static_cast<Note>(static_cast<uint8_t>(tonic_) + intervals.first[i]);
    }
}

uint8_t Scale::getNearestScaleMidiNote(uint8_t v, int offset ){
    int octave = getMidiOctave(v) ;
    uint8_t uNote = static_cast<uint8_t>(getMidiNoteName(v));

    for (int i = 0; i < notesSize_ ; ++i ){
        if ( uNote <= static_cast<uint8_t>(notes_[i]) ){
            int idx = i + offset ;
            int mIdx = modulo(idx,notesSize_);

            // adjust octave if offset rolls over/under
            if( idx != mIdx ) octave += std::floor(static_cast<float>(idx) / notesSize_) ;

            return getMidiValue(notes_[mIdx], octave) ;
        }
    }

    return getMidiValue(notes_[notesSize_ - 1], octave);
}

int Scale::getMidiOctave(uint8_t v){
    return ( v / 12 ) - 1 ;
}

Note Scale::getMidiNoteName(uint8_t v){
    return static_cast<Note>( v % 12 ) ;
}

uint8_t Scale::getMidiValue(Note n, int octave){
    int i = static_cast<uint8_t>(n) + (12 * (octave + 1) );
    if ( i < 0 || i > 127 ) return CONFIG_NULL_MIDI_VALUE ;

    return i ;
}

int Scale::modulo(int a, int b){
    return (a % b + b) % b ;
}