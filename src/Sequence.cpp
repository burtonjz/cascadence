#include "Sequence.hpp"
#include "MidiController.hpp"
#include "config.hpp"
#include "scale.hpp"
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

Sequence::Sequence(const double* sampleRate):
    sampleRate_(sampleRate),
    map_(nullptr),
    controller_ptr_(nullptr),
    pattern_(),
    startFrames(),
    endFrames(),
    Scale_(),
    root_(),
    frame_(0),
    bpm_(CONFIG_DEFAULT_BPM)
{
}

Sequence::Sequence(const double* sampleRate, Scale scale, int bpm, SequencePattern pattern):
    sampleRate_(sampleRate),
    map_(nullptr),
    controller_ptr_(nullptr),
    pattern_(),
    startFrames(),
    endFrames(),
    Scale_(scale),
    root_(),
    frame_(0),
    bpm_(bpm)
{
    setPattern(pattern);
}

void Sequence::initialize(LV2_URID_Map* map){
    map_ = map ;
}

void Sequence::onParameterChanged(const StateMapItem* item){
    if ( item->urid == map_->map(map_->handle, CASCADENCE__bpm)){
        LV2_Atom_Int* atom = reinterpret_cast<LV2_Atom_Bool*>(item->value);
        setBpm(atom->body) ;
    }
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

void Sequence::setStatus(bool status){
    if (status){
        root_.msg[0] = LV2_MIDI_MSG_NOTE_ON ;
    } else {
        root_.msg[0] = LV2_MIDI_MSG_NOTE_OFF ;
    }
}

const MidiNoteEvent Sequence::getRootNote() const {
    return root_ ;
}

void Sequence::sequenceMidiNoteEvents(){
    MidiNoteEvent out ;

    for(size_t i = 0 ; i < pattern_.size() ; ++i ){
        auto n = pattern_.getSequenceNote(i);
        if ( root_.msg[0] == LV2_MIDI_MSG_NOTE_ON && frame_ == startFrames[i]){
            uint8_t v = Scale_.getNearestScaleMidiNote(root_.msg[1], n.note) ;
            if ( v != CONFIG_NULL_MIDI_VALUE && !controller_ptr_->isMidiOn(v) ) {
                out.event = root_.event ;
                out.msg[0] = LV2_MIDI_MSG_NOTE_ON ;
                out.msg[1] = v ;
                out.msg[2] = root_.msg[2] ;
                if(controller_ptr_) controller_ptr_->append(out);
            }
        } else if ( frame_ == endFrames[i]){
            uint8_t v = Scale_.getNearestScaleMidiNote(root_.msg[1], n.note) ;
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

void Sequence::handleMidiNoteEvent(const MidiNoteEvent m){
    /*
    CASES
    1. ON MSG  + New Root Note: set for first time and start sequencing
    2. OFF MSG + current Root Note, update and sequencing stops
    3. ON MSG + NEW Root Note: update root note, reset frame, start sequencing
    4. OFF MSG, doesn't match root note: do nothing
    */
    switch(m.msg[0]){
    case LV2_MIDI_MSG_NOTE_ON:
        root_ = m ;
        frame_ = 0 ;
        break ;
    case LV2_MIDI_MSG_NOTE_OFF:
        if ( root_.msg[1] == m.msg[1] ) {
            root_ = m ;
            frame_ = 0 ;
        }
        break ;
    default:
        break ;
    }
}

void Sequence::calculateFrameTiming(){
    int framesPerBeat = 60.0 * *sampleRate_ / bpm_ ;
    duration_ = pattern_.getDuration() * framesPerBeat ;
    for (size_t i = 0 ; i < pattern_.size() ; ++i ){
        auto n = pattern_.getSequenceNote(i);
        startFrames[i] = n.start * framesPerBeat ;
        endFrames[i] = n.end * framesPerBeat ;
    }
}

void Sequence::tick(){
    if ( root_.msg[0] == LV2_MIDI_MSG_NOTE_ON ){
        frame_ += 1 ;
        if ( frame_ > duration_ ){
            frame_ = 0 ;
        }
    }
}
