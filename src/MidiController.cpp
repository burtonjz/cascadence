#include "MidiController.hpp"
#include "Sequence.hpp"

#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <cstddef>
#include <iostream>


MidiController::MidiController():
    sequence_ptr_(nullptr),
    input_(nullptr),
    output_(nullptr),
    capacity_(0),
    activeNotes_()
{
    activeNotes_.fill(CONFIG_NULL_MIDI_VALUE);
}

void MidiController::setSequence(Sequence* ptr){
    sequence_ptr_ = ptr ;
}

void MidiController::append(MidiNoteEvent m){
    updateActive(m);
    lv2_atom_sequence_append_event(
        output_,
        capacity_,
        &m.event 
    );
}

void MidiController::setInput(LV2_Atom_Sequence* midiOut){
        input_ = midiOut ;
}

const LV2_Atom_Sequence* MidiController::getInput() const {
    return input_ ;
}

void MidiController::setOutput(LV2_Atom_Sequence* midiOut){
        output_ = midiOut ;
}

void MidiController::prepareBuffer(){
    if(!output_) return ;
    
    capacity_ = output_->atom.size ;
    lv2_atom_sequence_clear(output_);
    output_->atom.type = input_->atom.type ;

}

void MidiController::processInput(LV2_Atom_Event* ev){
    const uint8_t* const midiMsg = reinterpret_cast<const uint8_t*>(ev + 1);
    const LV2_Midi_Message_Type midiType = lv2_midi_message_type(midiMsg);

    switch(midiType){
            case LV2_MIDI_MSG_NOTE_ON:
            case LV2_MIDI_MSG_NOTE_OFF:
                // capture midi note event
                MidiNoteEvent m ;
                m.event.time.frames = ev->time.frames ;
                m.event.body.type = ev->body.type ;
                m.event.body.size = ev->body.size ;
                m.msg[0] = midiMsg[0] ;
                m.msg[1] = midiMsg[1] ;
                m.msg[2] = midiMsg[2] ;

                if(sequence_ptr_) sequence_ptr_->setRootNote(m) ;

                break ;
            default:
                // Forward all other midi events
                lv2_atom_sequence_append_event(output_, capacity_, ev);
                break ;
            }
}

bool MidiController::isMidiOn(uint8_t midiVal){
    for ( int i = 0 ; i < activeNotes_.size() ; ++i ){
        if ( activeNotes_[i] == midiVal ) return true ;
    }
    return false ;
}

void MidiController::appendAllMidiOff(MidiNoteEvent root){
    for (int i = 0 ; i < activeNotes_.size() ; ++i ){
        if ( activeNotes_[i] != CONFIG_NULL_MIDI_VALUE && sequence_ptr_ ){
            MidiNoteEvent m ;
            m.event.time.frames = root.event.time.frames ;
            m.event.body.type = root.event.body.type ;
            m.event.body.size = root.event.body.size ;
            m.msg[0] = LV2_MIDI_MSG_NOTE_OFF ;
            m.msg[1] = activeNotes_[i] ;
            m.msg[2] = 0 ;
            append(m);
        }
    }
}

void MidiController::updateActive(MidiNoteEvent m){
    switch(m.msg[0]){
        case LV2_MIDI_MSG_NOTE_ON:
            for ( int i = 0 ; i < activeNotes_.size() ; ++i ){
                if ( activeNotes_[i] == CONFIG_NULL_MIDI_VALUE ){
                    activeNotes_[i] = m.msg[1] ;
                    return ;
                }
            }
            std::cout << "[MidiController] WARN: no null midi notes in array, could not append " << static_cast<int>(m.msg[1]) << std::endl ;
            break ;
        case LV2_MIDI_MSG_NOTE_OFF:
            for ( int i = 0 ; i < activeNotes_.size() ; ++i ){
                if ( activeNotes_[i] == m.msg[1] ){
                    activeNotes_[i] = CONFIG_NULL_MIDI_VALUE ;
                    return ;
                }
            }
            break ;
        default:
            return ;
    }
}
