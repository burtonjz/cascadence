#include "MidiController.hpp"
#include "Sequence.hpp"
#include "urids.hpp"

#include <lv2/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <iostream>


MidiController::MidiController():
    input_(nullptr),
    output_(nullptr),
    map_(nullptr),
    sequence_(nullptr),
    capacity_(0),
    activeNotes_()
{
    activeNotes_.fill(CONFIG_NULL_MIDI_VALUE);
}

void MidiController::initialize(LV2_URID_Map* map){
    map_ = map ;
}

void MidiController::setSequence(Sequence* ptr){
    sequence_ = ptr ;
}

void MidiController::onParameterChanged(const StateMapItem* item){
    if ( item->urid == map_->map(map_->handle, CASCADENCE__bypass)){
        LV2_Atom_Bool* atom = reinterpret_cast<LV2_Atom_Bool*>(item->value);
        bool bypassed = atom->body ;
        // on bypass, turn all notes off, set sequence to off
        appendAllMidiOff();
        if (bypassed) sequence_->setStatus(false);
    }
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

                if(sequence_){
                    /*
                    The Midi Controller Needs to clear active midi notes in two cases

                    1. New Midi Note On is received
                    2. Midi Note off Corresponds with current root midi note
                    */
                    if (midiType == LV2_MIDI_MSG_NOTE_ON){
                        appendAllMidiOff();
                    } else {
                        const MidiNoteEvent root = sequence_->getRootNote() ;
                        if (m.msg[1] == root.msg[1]){
                            appendAllMidiOff();
                        }
                    }

                    sequence_->handleMidiNoteEvent(m);
                }

                break ;
            default:
                // Forward all other midi events
                lv2_atom_sequence_append_event(output_, capacity_, ev);
                break ;
            }
}

void MidiController::passInput(LV2_Atom_Event* ev){
    const uint8_t* const midiMsg = reinterpret_cast<const uint8_t*>(ev + 1);
    MidiNoteEvent m ;
    m.event.time.frames = ev->time.frames ;
    m.event.body.type = ev->body.type ;
    m.event.body.size = ev->body.size ;
    m.msg[0] = midiMsg[0] ;
    m.msg[1] = midiMsg[1] ;
    m.msg[2] = midiMsg[2] ;
    updateActive(m);
    lv2_atom_sequence_append_event(output_, capacity_, ev);
}

bool MidiController::isMidiOn(uint8_t midiVal){
    for ( size_t i = 0 ; i < activeNotes_.size() ; ++i ){
        if ( activeNotes_[i] == midiVal ) return true ;
    }
    return false ;
}

void MidiController::appendAllMidiOff(){
    MidiNoteEvent root = sequence_->getRootNote() ;
    for ( size_t i = 0 ; i < activeNotes_.size() ; ++i ){
        if ( activeNotes_[i] != CONFIG_NULL_MIDI_VALUE && sequence_ ){
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
            for ( size_t i = 0 ; i < activeNotes_.size() ; ++i ){
                if ( activeNotes_[i] == CONFIG_NULL_MIDI_VALUE ){
                    activeNotes_[i] = m.msg[1] ;
                    return ;
                }
            }
            std::cout << "[MidiController] WARN: no null midi notes in array, could not append " << static_cast<int>(m.msg[1]) << std::endl ;
            break ;
        case LV2_MIDI_MSG_NOTE_OFF:
            for ( size_t i = 0 ; i < activeNotes_.size() ; ++i ){
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
