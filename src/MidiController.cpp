#include "MidiController.hpp"
#include "Sequence.hpp"

#include <cstddef>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>


MidiController::MidiController():
    sequence_ptr_(nullptr),
    input_(nullptr),
    output_(nullptr),
    capacity_(0)
{}

void MidiController::setSequence(Sequence* ptr){
    sequence_ptr_ = ptr ;
}

void MidiController::append(MidiNoteEvent m){
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