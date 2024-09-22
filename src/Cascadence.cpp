#include "Cascadence.hpp"
#include "MidiController.hpp"
#include "SequencePattern.hpp"
#include "config.hpp"
#include "portInfo.hpp"
#include "urids.hpp"

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/ext/log/logger.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/midi/midi.h>

Cascadence::Cascadence(const double sampleRate, const LV2_Feature *const *features):
    // logger_(),
    uridMap_(nullptr),
    sampleRate_(sampleRate),
    samplePeriod_(1.0/sampleRate),
    forge_(),
    features_(features),
    bpm_(CONFIG_DEFAULT_BPM),
    Sequence_(&sampleRate_),
    MidiController_(),
    ParamController_()
{
    // check features
    const char* missing = lv2_features_query(
        features_,
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
    lv2_atom_forge_init(&forge_,uridMap_);

    MidiController_.initialize(uridMap_);
    ParamController_.initialize(features_, &forge_, uridMap_, &urids_);


    // Define Sequence Pattern
    // Scale* scale = Sequence_.getScale() ;
    // scale->setScale(Note::D, ScaleType::MAJOR) ;
    // SequencePattern pattern ;
    // pattern.setSize(5) ;
    // pattern.duration = 6.0 ;
    // pattern.setArray<int>(pattern.notes, {0,2,4,0,4});
    // pattern.setArray<float>(pattern.start, {0,1,2,3,3});
    // pattern.setArray<float>(pattern.end, {0.5,1.5,2.5,5,5});
    // Sequence_.setBpm(120);

    // Megalovania Sequence Definition
    Scale* scale = Sequence_.getScale() ;
    scale->setScale(Note::D,ScaleType::CHROMATIC);
    SequencePattern pattern ;
    pattern.append(0, 0.0, 0.5);
    pattern.append(0, 0.5, 1.0);
    pattern.append(12, 1.0, 2.0);
    pattern.append(7, 2.0, 3.5);
    pattern.append(6, 3.5, 4.5);
    pattern.append(5, 4.5, 5.5);
    pattern.append(3, 5.5, 6.5);
    pattern.append(0, 6.5, 7.0);
    pattern.append(3, 7.0, 7.5);
    pattern.append(5, 7.5, 8.0);

    Sequence_.setBpm(214);

    Sequence_.setPattern(pattern);
    Sequence_.setMidiController(&MidiController_);
    MidiController_.setSequence(&Sequence_);
}

void Cascadence::onParameterChanged(const StateMapItem* item){
    if ( item->urid == uridMap_->map(uridMap_->handle, CASCADENCE__bypass)){
        LV2_Atom_Bool* atom = reinterpret_cast<LV2_Atom_Bool*>(item->value);
        bypassed_ = atom->body ;
    }
}

ParameterController* Cascadence::getParameterController(){
    return &ParamController_ ;
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
    // register observers (also sends initial notifies to sync)
    ParamController_.registerObserver(this);
    ParamController_.registerObserver(&MidiController_);

    return ;
}

void Cascadence::run(const uint32_t sampleCount){
    MidiController_.prepareBuffer();

    // loop through incoming sequence events
    uint32_t lastFrame = 0 ;
    LV2_ATOM_SEQUENCE_FOREACH(MidiController_.getInput(), ev){
        const uint32_t frame = ev->time.frames;

        // process frames up to this event
        if (!bypassed_){
            sequence(lastFrame, frame);
        }

        lastFrame = frame ;

        // handle parameter/patch events
        if (lv2_atom_forge_is_object_type(&forge_, ev->body.type)){
            const LV2_Atom_Object* obj = reinterpret_cast<const LV2_Atom_Object*>(&ev->body);
            if (ParamController_.isPatchEvent(obj)){
                ParamController_.handleEvent(obj,ev->time.frames);
            }
        }
        // handle midi events
        else if (ev->body.type == urids_.midiEvent){
            if (bypassed_) MidiController_.passInput(ev);
            else MidiController_.processInput(ev);
        };
    }

    // sequence remaining frames in buffer
    if (!bypassed_){
        sequence(lastFrame, sampleCount);
    }
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
