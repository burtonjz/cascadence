#include "Cascadence.hpp"
#include "config.hpp"
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
    sampleRate_(sampleRate),
    samplePeriod_(1.0/sampleRate),
    midiOutCapacity_(0),
    currentPressed_(),
    isPressed_(false),
    framesSincePressed_(0),
    bpm_(CONFIG_DEFAULT_BPM)
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
    midiOutCapacity_ = midiOut_->atom.size ;
    lv2_atom_sequence_clear(midiOut_);
    midiOut_->atom.type = midiIn_->atom.type ;

    // loop through incoming midi events
    uint32_t lastFrame = 0 ;
    LV2_ATOM_SEQUENCE_FOREACH(midiIn_, ev){
        const uint32_t frame = ev->time.frames;
        sequence(lastFrame, frame);
        lastFrame = frame ;
        if (ev->body.type == urids_.midiEvent) processMidi(ev);
    }

    // sequence remaining frames in buffer
    sequence(lastFrame, sampleCount);
}

void Cascadence::tick(){
    framesSincePressed_ += 1 ;
}

void Cascadence::sequence(const uint32_t start, const uint32_t end){
    /* 
    proof of concept with a basic 1-3-5 arpeggio.
    need to convert sampling frequency to "frames per beat" then modulus to play the appropriate note in the sequence
    e.g, "44100 hz * (60s/1m) * (1m/120beats) = one note every 22050 frames (for quarter notes)"
    we will turn the last not off the same time we turn the current note on
    */
    // std::cout << isPressed_ << std::endl ;
    // if (!isPressed_) return ;
    int framesPerBeat = ( 60.0 * sampleRate_ / bpm_ );
    int sequenceLength = 3 ; // 3 notes in our repeating sequence

    for (uint32_t i = start; i < end; ++i){
        if (isPressed_ && framesSincePressed_ % framesPerBeat == 0){
            int sequenceIndex = ( framesSincePressed_ / framesPerBeat ) % sequenceLength ;
            if (sequenceIndex == 0){
                appendMidi(i); // current note
                appendMidi(i,7,0,LV2_MIDI_MSG_NOTE_OFF); // terminate perfect 5th
            } 
            else if (sequenceIndex == 1){
                appendMidi(i,4); // major 3rd
                appendMidi(i,0,0,LV2_MIDI_MSG_NOTE_OFF); // terminate current note
            }
            else if (sequenceIndex == 2){
                appendMidi(i,7); // perfect 5th
                appendMidi(i,4,0,LV2_MIDI_MSG_NOTE_OFF); // terminate major 3rd
            } 
        }
        tick();
    }
}

void Cascadence::processMidi(LV2_Atom_Event* ev){
    const uint8_t* const midiMsg = reinterpret_cast<const uint8_t*>(ev + 1);
    const LV2_Midi_Message_Type midiType = lv2_midi_message_type(midiMsg);

    switch(midiType){
            case LV2_MIDI_MSG_NOTE_ON:
                std::cout << "Received midi note on frame " << ev->time.frames << std::endl ;
                // capture midi note event
                currentPressed_.event.time.frames = ev->time.frames ;
                currentPressed_.event.body.type = ev->body.type ;
                currentPressed_.event.body.size = ev->body.size ;
                currentPressed_.msg[0] = midiMsg[0] ;
                currentPressed_.msg[1] = midiMsg[1] ;
                currentPressed_.msg[2] = midiMsg[2] ;
                isPressed_ = true ;
                framesSincePressed_ = 0 ;
                break ;

            case LV2_MIDI_MSG_NOTE_OFF:
                std::cout << "Received midi note off frame " << ev->time.frames << std::endl ;
                if (midiMsg[1] == currentPressed_.msg[1]){
                    isPressed_ = false ;
                    appendMidi(ev->time.frames,0,0,LV2_MIDI_MSG_NOTE_OFF); // terminate root
                    appendMidi(ev->time.frames,4,0,LV2_MIDI_MSG_NOTE_OFF); // terminate major 3rd
                    appendMidi(ev->time.frames,7,0,LV2_MIDI_MSG_NOTE_OFF); // terminate perfect 5th
                }
                break ;
            default:
                // Forward all other midi events
                lv2_atom_sequence_append_event(midiOut_, midiOutCapacity_, ev);
                break ;
            }

}

void Cascadence::deactivate(){
    return ;
}

void Cascadence::appendMidi(int frame, int dSemitones, int dVelocity, LV2_Midi_Message_Type status){
    if ( !isMidiInBounds(currentPressed_.msg[1],dSemitones)){
        // lv2_log_note(&logger_, "requested midiNote out of midi range. Will not append.");
        return ;
    }
    if ( !isMidiInBounds(currentPressed_.msg[2],dVelocity)){
        // TODO: likely a more elegant solution? later on we'll want to fade notes in an arpeggiator and don't want them to drop,
        // though maybe that's handled elsewhere using a squeeze style function?
        // lv2_log_note(&logger_, "requested velocity out of midi range. Will not append."); 
        return ;
    }
    //TODO: determine if atom_sequence requires a max/min frame timestamp (perhaps capped by 0 and sample count of buffer?)

    MidiNoteEvent note ;
    note.event.time.frames = frame ;
    note.event.body.type = currentPressed_.event.body.type ;
    note.event.body.size = currentPressed_.event.body.size ;

    note.msg[0] = status ;
    note.msg[1] = currentPressed_.msg[1] + dSemitones ;
    note.msg[2] = currentPressed_.msg[2] + dVelocity ; 
    std::cout << "sending output midi: " << static_cast<int>(note.msg[0]) << "," << static_cast<int>(note.msg[1]) << "," << static_cast<int>(note.msg[2]) << std::endl ;

    /* 
    Note: I'm pretty sure the note.msg is getting included because it's just the memory address
    after the note.event, and the body.size tells us to look for that data.
    */
    lv2_atom_sequence_append_event(midiOut_, midiOutCapacity_, &note.event );

}

bool Cascadence::isMidiInBounds(uint8_t midiVal, int d ){
    int sum = static_cast<int>(midiVal) + d ;
    return sum >= 0 && sum <= 127 ;
}