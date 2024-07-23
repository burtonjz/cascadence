#include "Sequence.hpp"
#include "MidiController.hpp"
#include "config.hpp"
#include "scale.hpp"
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>
#include <iostream>

Sequence::Sequence(const double* sampleRate):
    sampleRate_(sampleRate),
    controller_ptr_(nullptr),
    pattern_(),
    startFrames(),
    endFrames(),
    isPressed_(false),
    Scale_(),
    root_(),
    frame_(0),
    bpm_(CONFIG_DEFAULT_BPM)
{
}

Sequence::Sequence(const double* sampleRate, Scale scale, int bpm, SequencePattern pattern):
    sampleRate_(sampleRate),
    controller_ptr_(nullptr),
    pattern_(),
    startFrames(),
    endFrames(),
    isPressed_(false),
    Scale_(scale),
    root_(),
    frame_(0),
    bpm_(bpm)
{    
    setPattern(pattern);
}

void Sequence::setMidiController(MidiController* ptr){
    controller_ptr_ = ptr ;
}

void Sequence::setPattern(SequencePattern pattern){
    pattern_ = pattern ;
    calculateFrameTiming() ;
}

Scale* Sequence::getScale(){
    return &Scale_ ;
}

void Sequence::setBpm(int bpm){
    bpm_ = bpm ;
    calculateFrameTiming() ;
}

void Sequence::setMidiStatus(LV2_Midi_Message_Type midiStatus){
    root_.msg[0] = midiStatus ;
}

void Sequence::setRootNote(MidiNoteEvent m){
    root_ = m ;
    switch(root_.msg[0]){
    case LV2_MIDI_MSG_NOTE_ON:
        isPressed_ = true ;
        frame_ = 0 ;
        break ;
    case LV2_MIDI_MSG_NOTE_OFF:
        isPressed_ = false ;
        frame_ = 0 ;
        if (controller_ptr_){
            controller_ptr_->appendAllMidiOff() ;
        }
        break ;
    default:
        break ;
    }
}

const MidiNoteEvent Sequence::getRootNote() const {
    return root_ ;
}

void Sequence::sequenceMidiNoteEvents(){
    MidiNoteEvent out ;

    for(int j = 0 ; j < pattern_.length ; ++j ){
        if ( isPressed_ && frame_ == startFrames[j]){
            uint8_t v = Scale_.getNearestScaleMidiNote(root_.msg[1], pattern_.notes[j]) ;
            if ( v != CONFIG_NULL_MIDI_VALUE && !controller_ptr_->isMidiOn(v) ) {
                out.event = root_.event ;
                out.msg[0] = LV2_MIDI_MSG_NOTE_ON ;
                out.msg[1] = v ;
                out.msg[2] = root_.msg[2] ;
                if(controller_ptr_) controller_ptr_->append(out);
            }
        } else if ( frame_ == endFrames[j]){
            uint8_t v = Scale_.getNearestScaleMidiNote(root_.msg[1], pattern_.notes[j]) ;
            if ( v != CONFIG_NULL_MIDI_VALUE && controller_ptr_->isMidiOn(v) ){
                out.event = root_.event ;
                out.msg[0] = LV2_MIDI_MSG_NOTE_OFF ;
                out.msg[1] = v ;
                out.msg[2] = root_.msg[2] ;
                if(controller_ptr_) controller_ptr_->append(out);
            }
        }
    }
}

void Sequence::calculateFrameTiming(){
    int framesPerBeat = 60.0 * *sampleRate_ / bpm_ ;
    std::cout << "sample rate = " << *sampleRate_ << " bpm = " << bpm_ << " framesPerBeat = " << framesPerBeat << std::endl ;
    for (int i = 0 ; i < pattern_.length ; ++i ){
        if ( i == 0 ) startFrames[i] = 0.0 ;
        else startFrames[i] = endFrames[i-1] ;
        endFrames[i] = startFrames[i] + framesPerBeat * pattern_.durations[i] ;
    }
}
void Sequence::tick(){
    frame_ += 1 ;
    if ( frame_ > endFrames[pattern_.length - 1] ){
        frame_ = 0 ;
    }
}