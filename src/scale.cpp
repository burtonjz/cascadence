#include "scale.hpp"
#include "config.hpp"

#include <cmath>
#include <algorithm>

Scale::Scale(Note tonic, ScaleType typ):
    scaleNotes_()
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

const std::pair<uint8_t*, size_t> Scale::getScale(){
    return std::make_pair(scaleNotes_.data(), scaleLength_);
}

void Scale::populateScale(){
    // if scale isn't set, create dummy array
    if (tonic_ == Note::NULL_NOTE){
        for (int i = 0; i < scaleLength_; ++i){
            scaleNotes_[i] = CONFIG_NULL_MIDI_VALUE ;
        }
        return ;
    }

    // otherwise, populate scale based off the scale intervals
    auto intervals = getScaleIntervals(type_);
    scaleLength_ = intervals.second ;
    for (size_t i = 0 ; i < scaleLength_ ; ++i ){
        scaleNotes_[i] = modulo(static_cast<uint8_t>(tonic_) + intervals.first[i], 12);
    }
    std::sort(scaleNotes_.begin(),scaleNotes_.begin() + scaleLength_);
}

uint8_t Scale::getNearestScaleMidiNote(uint8_t v, int offset ){
    uint8_t baseInput = modulo(v,12);
    uint8_t inputOctave = (v / 12) - 1 ;

    // find closest input index
    uint8_t* it = std::lower_bound(scaleNotes_.begin(), scaleNotes_.begin() + scaleLength_ - 1, baseInput);
    int closestIdx = std::distance(scaleNotes_.begin(), it);

    // calculate output index
    size_t outputIdx = modulo(closestIdx + offset, static_cast<int>(scaleLength_)) ;

    // calculate output octave
    int outputOctave = inputOctave + (closestIdx + offset) / static_cast<float>(scaleLength_);

    uint8_t output = static_cast<uint8_t>(scaleNotes_[outputIdx]) + 12 * (outputOctave + 1) ;

    return output ;
}

int Scale::modulo(int a, int b){
    return (a % b + b) % b ;
}