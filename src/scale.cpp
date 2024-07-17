#include "scale.hpp"
#include "config.hpp"

#include <cmath>
#include <algorithm>
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
    return std::make_pair(notes_.data(), notesSize_);
}

const std::pair<uint8_t*, size_t> Scale::getUScale(){
    return std::make_pair(uNotes_.data(), notesSize_);
}

void Scale::populateScale(){
    // if scale isn't set, create dummy array
    if (tonic_ == Note::NULL_NOTE){
        for (int i = 0; i < notesSize_; ++i){
            uNotes_[i] = CONFIG_NULL_MIDI_VALUE ;
            notes_[i] = Note::NULL_NOTE ;
        }
        return ;
    }

    // otherwise, populate scale based off the scale intervals
    auto intervals = getScaleIntervals(type_);
    notesSize_ = intervals.second ;
    size_t i = 0 ;
    
    for (; i < notesSize_ ; ++i ){
        uNotes_[i] = static_cast<uint8_t>(tonic_) + intervals.first[i] ;
        notes_[i] = static_cast<Note>(uNotes_[i]);
    }
}

// midi=61 noteID=1 offset=0 offsetNoteID=2 idx=0 mIdx=0 octave=4 outputMidi=62
uint8_t Scale::getNearestScaleMidiNote(uint8_t v, int offset ){
    Note inputNote = getMidiNoteName(v);

    // find closest midi value in scale array
    uint8_t* it = std::lower_bound(uNotes_.begin(), uNotes_.begin() + notesSize_ - 1, static_cast<uint8_t>(inputNote));
    size_t inputIdx = std::distance(uNotes_.begin(), it);

    // get the index of the offset value
    size_t outputIdx = inputIdx + offset ;
    size_t outputBoundedIdx = modulo(outputIdx, notesSize_);

    // get the appropriate octave
    int octave = getMidiOctave(v) ;
    if( outputIdx != outputBoundedIdx ) octave += std::floor(static_cast<float>(outputIdx) / notesSize_) ; 

    uint8_t offsetValue = uNotes_[outputBoundedIdx] - static_cast<uint8_t>(tonic_);
    Note outputNote = getMidiNoteName(v + offsetValue);

    return getMidiValue(outputNote,octave);
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