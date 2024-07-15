#include "Sequence.hpp"
#include "MidiController.hpp"
#include <iostream>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>


Sequence::Sequence(const double* sampleRate):
    sampleRate_(sampleRate),
    controller_ptr_(nullptr),
    pattern_(),
    startFrames(),
    endFrames(),
    isPressed_(false),
    Scale_(),
    Root_(),
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
    Root_(),
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

    int framesPerBeat = 60.0 * *sampleRate_ / bpm_ ;

    for (int i = 0 ; i < pattern_.length ; ++i ){
        if ( i == 0 ) startFrames[i] = 0.0 ;
        else startFrames[i] = endFrames[i-1] ;
        endFrames[i] = startFrames[i] + framesPerBeat * pattern_.durations[i] ;
        std::cout << "index: " << i << ", startFrame: " << startFrames[i] << ", endFrame: " << endFrames[i] << std::endl ; 
    }
}

void Sequence::setMidiStatus(LV2_Midi_Message_Type midiStatus){
    Root_.msg[0] = midiStatus ;
}

void Sequence::setRootNote(MidiNoteEvent m){
    std::cout << "Setting Root Note." << std::endl ;
    Root_ = m ;
    switch(Root_.msg[0]){
    case LV2_MIDI_MSG_NOTE_ON:
        isPressed_ = true ;
        frame_ = 0 ;
        break ;
    case LV2_MIDI_MSG_NOTE_OFF:
        isPressed_ = false ;
        frame_ = 0 ;
        break ;
    default:
        break ;
    }
}

void Sequence::sequenceMidiNoteEvents(){
    MidiNoteEvent out ;

    for(int j = 0 ; j < pattern_.length ; ++j ){
        if ( isPressed_ && frame_ == startFrames[j]){
            out.event = Root_.event ;
            out.msg[0] = LV2_MIDI_MSG_NOTE_ON ;
            out.msg[1] = Root_.msg[1] + pattern_.notes[j] ;
            out.msg[2] = Root_.msg[2] ;
            std::cout << "appending event: " << static_cast<int>(out.msg[0]) << " " << static_cast<int>(out.msg[1]) << " " << static_cast<int>(out.msg[2]) << std::endl ;
            if(controller_ptr_) controller_ptr_->append(out);
        } else if ( frame_ == endFrames[j]){
            out.event = Root_.event ;
            out.msg[0] = LV2_MIDI_MSG_NOTE_OFF ;
            out.msg[1] = Root_.msg[1] + pattern_.notes[j] ;
            out.msg[2] = Root_.msg[2] ;
            std::cout << "appending event: " << static_cast<int>(out.msg[0]) << " " << static_cast<int>(out.msg[1]) << " " << static_cast<int>(out.msg[2]) << std::endl ;
            if(controller_ptr_) controller_ptr_->append(out);
        }
    }
}

void Sequence::tick(){
    frame_ += 1 ;
    if ( frame_ > endFrames[pattern_.length - 1] ){
        frame_ = 0 ;
    }
}