#include "Sequence.hpp"
#include "MidiController.hpp"
#include "config.hpp"
#include "scale.hpp"
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

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
            controller_ptr_->appendAllMidiOff(getRootNote()) ;
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

    for(int j = 0 ; j < pattern_.size() ; ++j ){
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
    duration_ = pattern_.duration * framesPerBeat ;
    for (int i = 0 ; i < pattern_.size() ; ++i ){
        startFrames[i] = pattern_.start[i] * framesPerBeat ;
        endFrames[i] = pattern_.end[i] * framesPerBeat ;
    }
}
void Sequence::tick(){
    frame_ += 1 ;
    if ( frame_ > duration_ ){
        frame_ = 0 ;
    }
}