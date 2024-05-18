#include "Cascadence.hpp"
#include "portInfo.hpp"


#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

#include <iostream>

Cascadence::Cascadence(const double sampleRate, const LV2_Feature *const *features):
    // logger_(),
    midiIn_(nullptr),
    midiOut_(nullptr),
    uridMap_(nullptr),
    sampleRate_(sampleRate)
{
    const char* missing = lv2_features_query(
        features,
        // LV2_LOG__log, &logger_.log, false,
        LV2_URID__map, &uridMap_, true,
        NULL
    );

    // lv2_log_logger_set_map(&logger_,uridMap_);
    if(missing){
        // lv2_log_error(&logger_,"Missing feature <%s>/n", missing);
        throw ;
    }

    urids_.initialize(uridMap_);

}

void Cascadence::connectPort(const uint32_t port, void* data){
    switch(port){
        case PORT_MIDI_IN:
            midiIn_ = static_cast<const LV2_Atom_Sequence*>(data);
            break ;
        case PORT_MIDI_OUT:
            midiOut_ = static_cast<LV2_Atom_Sequence*>(data);
            break ;
        default:
            break ;
    }
}

void Cascadence::activate(){
    return ;
}

void Cascadence::run(const uint32_t sampleCount){
    // prep atom sequence
    const uint32_t outCapacity = midiOut_->atom.size ;
    
    lv2_atom_sequence_clear(midiOut_);
    midiOut_->atom.type = midiIn_->atom.type ;

    // loop through incoming midi events
    LV2_ATOM_SEQUENCE_FOREACH(midiIn_, ev){
        if (ev->body.type == urids_.midiEvent){
            // lv2_log_note(&logger_,"midi event received!");
            std::cout << "midi event received!" << std::endl ;

            const uint8_t* const midiMsg = reinterpret_cast<const uint8_t*>(ev + 1);
            const LV2_Midi_Message_Type midiType = lv2_midi_message_type(midiMsg);

            switch(midiType){
            case LV2_MIDI_MSG_NOTE_ON:
            case LV2_MIDI_MSG_NOTE_OFF:
                // First, forward the original midi event
                // lv2_log_note(&logger_,"forwarding midi event");
                lv2_atom_sequence_append_event(midiOut_, outCapacity, ev);

                // now, do other midi processing, our basic example will be adding a major triad
                // MidiNoteEvent ref ;
                // ref.event = *ev ;
                // ref.msg[0] = midiMsg[0] ;
                // ref.msg[1] = midiMsg[1] ;
                // ref.msg[2] = midiMsg[2] ;


                if(midiMsg[1] <= 127 - 4){ // must be in range of valid midi notes
                    MidiNoteEvent third ;

                    third.event.time.frames = ev->time.frames ; // same time
                    third.event.body.type   = ev->body.type ;
                    third.event.body.size   = ev->body.size ;

                    third.msg[0] = midiMsg[0]; // midi status
                    third.msg[1] = midiMsg[1] + 4 ; // pitch up 4 semitones (major 3rd)
                    third.msg[2] = midiMsg[2] ; // same velocity

                    // write event
                    lv2_atom_sequence_append_event(midiOut_, outCapacity, &third.event);
                }

                break ;
                

            default:
                // Forward all other midi events
                lv2_atom_sequence_append_event(midiOut_, outCapacity, ev);
                break ;
            }

        }
    }
}

void Cascadence::deactivate(){
    return ;
}

void Cascadence::appendMidi(MidiNoteEvent* mEvent, int dSemitones, int dFrames, int dVelocity){
    if(!mEvent){
        // lv2_log_error(&logger_, "Attempted to appendMidi for null atom event.");
        return ;
    }
    if ( !isMidiInBounds(mEvent->msg[1],dSemitones)){
        // lv2_log_note(&logger_, "requested midiNote out of midi range. Will not append.");
        return ;
    }
    if ( !isMidiInBounds(mEvent->msg[2],dVelocity)){
        // TODO: likely a more elegant solution? later on we'll want to fade notes in an arpeggiator and don't want them to drop,
        // though maybe that's handled elsewhere using a squeeze style function?
        // lv2_log_note(&logger_, "requested velocity out of midi range. Will not append."); 
        return ;
    }
    //TODO: determine if atom_sequence requires a max/min frame timestamp (perhaps capped by 0 and sample count of buffer?)



    


}

bool Cascadence::isMidiInBounds(uint8_t midiVal, int d ){
    int sum = static_cast<int>(midiVal) + d ;
    return sum >= 0 && sum <= 127 ;
}