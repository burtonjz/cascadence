#include "Cascadence.hpp"
#include "MidiController.hpp"
#include "SequencePattern.hpp"
#include "config.hpp"
#include "midiNoteEvent.hpp"
#include "portInfo.hpp"

#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

#include <iostream>

Cascadence::Cascadence(const double sampleRate, const LV2_Feature *const *features):
    // logger_(),
    uridMap_(nullptr),
    sampleRate_(sampleRate),
    samplePeriod_(1.0/sampleRate),
    bpm_(CONFIG_DEFAULT_BPM),
    Sequence_(&sampleRate_),
    MidiController_()
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

    // Testing rests and chords
    Scale* scale = Sequence_.getScale() ;
    scale->setScale(Note::D, ScaleType::MAJOR) ;
    SequencePattern pattern ;
    pattern.setSize(5) ;
    pattern.duration = 6.0 ;
    pattern.setArray<int>(pattern.notes, {0,2,4,0,4});
    pattern.setArray<float>(pattern.start, {0,1,2,3,3});
    pattern.setArray<float>(pattern.end, {0.5,1.5,2.5,5,5});

    
    Sequence_.setBpm(120);
    
    // Megalovania
    // Scale* scale = Sequence_.getScale() ;
    // scale->setScale(Note::D,ScaleType::CHROMATIC);
    // SequencePattern pattern ;
    // pattern.setSize(10) ;
    // pattern.setArray<int>(pattern.notes, {0,0,12,7,6,5,3,0,3,5});
    // pattern.setArray<float>(pattern.start, {0,0.5,1.0,2.0,3.5,4.5,5.5,6.5,7.0,7.5});
    // pattern.setArray<float>(pattern.end, {0.5,1,2,3.5,4.5,5.5,6.5,7.0,7.5,8.0});

    // Sequence_.setBpm(214);


    Sequence_.setPattern(pattern);
    Sequence_.setMidiController(&MidiController_);
    MidiController_.setSequence(&Sequence_);
}

void Cascadence::connectPort(const uint32_t port, void* data){
    switch(port){
        case PORT_MIDI_IN:
            MidiController_.setInput(static_cast<LV2_Atom_Sequence*>(data)) ;
            break ;
        case PORT_MIDI_OUT:
            MidiController_.setOutput(static_cast<LV2_Atom_Sequence*>(data)) ;
            break ;
        default:
            break ;
    }
}

void Cascadence::activate(){
    return ;
}

void Cascadence::run(const uint32_t sampleCount){
    MidiController_.prepareBuffer();

    // loop through incoming midi events
    uint32_t lastFrame = 0 ;
    LV2_ATOM_SEQUENCE_FOREACH(MidiController_.getInput(), ev){
        const uint32_t frame = ev->time.frames;
        sequence(lastFrame, frame);
        lastFrame = frame ;
        if (ev->body.type == urids_.midiEvent) MidiController_.processInput(ev);
    }

    // sequence remaining frames in buffer
    sequence(lastFrame, sampleCount);
}

void Cascadence::tick(){
    Sequence_.tick();
}

void Cascadence::sequence(const uint32_t start, const uint32_t end){
    for (uint32_t i = start; i < end; ++i){
        Sequence_.sequenceMidiNoteEvents() ;
        tick();
    }
}

void Cascadence::deactivate(){
    return ;
}

bool Cascadence::isMidiInBounds(uint8_t midiVal, int d ){
    int sum = static_cast<int>(midiVal) + d ;
    return sum >= 0 && sum <= 127 ;
}