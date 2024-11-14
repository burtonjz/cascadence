#include "SequenceNote.hpp"
#include "BDevices/MouseButton.hpp"

SequenceNote::SequenceNote(double x, double y, double width, double height, int nUnits):
    Button(),
    pos_(x,y),
    noteIndex_(0),
    startTime_(0),
    dragEdit_(false),
    dragUnits_(0)
{
    // set properties
    setActivatable(true);
    setClickable(true);
    setToggleable(true);
    setKeyPressable(false);
    setDraggable(true);

    // set widget position and size
    setPosition(pos_);
    setNumUnits(nUnits);
}

SequenceNote::SequenceNote():
    SequenceNote(0.0,0.0)
{}

BUtilities::Point<> SequenceNote::getPosition() const {
    return pos_ ;
}

void SequenceNote::setPosition(BUtilities::Point<> pt){
    pos_ = pt ;
    moveTo(pt);
}

int SequenceNote::getNumUnits() const {
    return numUnits_ ;
}

void SequenceNote::setNumUnits(int units){
    if ( units > 0 ){
        numUnits_ = units ;
        resize(UI_SEQUENCE_NOTE_UNIT_WIDTH * numUnits_, UI_SEQUENCE_NOTE_UNIT_HEIGHT);
    }
}

int SequenceNote::getNoteIndex() const {
    return noteIndex_ ;
}

void SequenceNote::setNoteIndex(int idx){
    noteIndex_ = idx ;
}

int SequenceNote::getStartIndex() const {
    return startTime_ ;
}

void SequenceNote::setStartIndex(int idx){
    startTime_ = idx ;
}

bool SequenceNote::isDragMode() const {
    return dragEdit_ ;
}

void SequenceNote::setDragMode(bool dragged){
    dragEdit_ = dragged ;
}

void SequenceNote::setDragTimeUnits(int units){
    dragUnits_ = units ;
}


void SequenceNote::updateToDragTime(){
    setNumUnits(dragUnits_) ;
}
