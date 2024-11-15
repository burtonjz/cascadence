#include "SequenceNote.hpp"
#include "BDevices/MouseButton.hpp"
#include "BStyles/Types/Color.hpp"
#include "BStyles/Types/ColorMap.hpp"
#include "BStyles/Types/Fill.hpp"
#include "ui/uiConfig.hpp"

SequenceNote::SequenceNote(double x, double y, int nUnits):
    Button(),
    pos_(x,y),
    noteIndex_(0),
    startTime_(0)
{
    // set properties
    setActivatable(true);
    setClickable(true);
    setToggleable(true);
    setKeyPressable(true);
    setDraggable(true);

    setFgColors(BStyles::blues);

    // set widget position and size
    setPosition(pos_);
    setNumUnits(nUnits);
    setValue(false);
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
    numUnits_ = units ;
    resize(UI_SEQUENCE_NOTE_UNIT_WIDTH * numUnits_, UI_SEQUENCE_NOTE_UNIT_HEIGHT);
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

inline void SequenceNote::onButtonPressed (BEvents::Event* event)
{
	Clickable::onButtonPressed (event);
}

inline void SequenceNote::onButtonReleased (BEvents::Event* event)
{
	Clickable::onButtonReleased (event);
}

inline void SequenceNote::onButtonClicked (BEvents::Event* event)
{
	Clickable::onButtonClicked (event);
}
