#include "note.hpp"

const std::array<Note, 17> notes = {
    Note::C     ,
    Note::CSHARP,
    Note::DFLAT ,
    Note::D     ,
    Note::DSHARP,
    Note::EFLAT ,
    Note::E     ,
    Note::F     ,
    Note::FSHARP,
    Note::GFLAT ,
    Note::G     ,
    Note::GSHARP,
    Note::AFLAT ,
    Note::A     ,
    Note::ASHARP,
    Note::BFLAT ,
    Note::B
};

const std::array<std::string, 17> noteStrings = {
    "C",
    "C#",
    "Db",
    "D" ,
    "D#",
    "Eb",
    "E" ,
    "F" ,
    "F#",
    "Gb",
    "G" ,
    "G#",
    "Ab",
    "A" ,
    "A#",
    "Bb",
    "B"
};

std::array<Note, 17> getNotes(){
    return notes ;
}

std::array<std::string, 17> getNoteStrings(){
    return noteStrings ;
}
