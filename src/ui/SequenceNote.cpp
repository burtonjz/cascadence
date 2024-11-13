#include "SequenceNote.hpp"
#include "BDevices/MouseButton.hpp"

SequenceNote::SequenceNote(double x, double y, double width, double height, int nUnits):
    Button(),
    pos_(x,y),
    unitSize_(width, height),
    numUnits_(nUnits),
    noteIndex_(0),
    startTime_(0),
    dragEdit_(false),
    dragUnits_(0)
{
    // set properties
    setActivatable(true);
    setClickable(true);
    setToggleable(false);
    setKeyPressable(false);
    setDraggable(true);

    setCallbackFunction(
        BEvents::Event::EventType::pointerDragEvent,
        [this](BEvents::Event* ev){draggedCallback(ev);}
    );

    setCallbackFunction(
        BEvents::Event::EventType::buttonReleaseEvent,
        [this](BEvents::Event* ev){unclickedCallback(ev);}
    );

    // set widget position and size
    setUnitSize(unitSize_.first, unitSize_.second);
    setPosition(pos_);
}

SequenceNote::SequenceNote():
    SequenceNote(0.0,0.0)
{}

void SequenceNote::setUnitSize(const double width, const double height){
    unitSize_ = std::make_pair(width, height);
    resize(unitSize_.first * numUnits_ , unitSize_.second);
}

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
        resize(unitSize_.first * numUnits_, unitSize_.second);
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

void SequenceNote::draggedCallback(BEvents::Event* event){
    if ( !event ) return ;
    if ( event->getEventType() != BEvents::Event::EventType::pointerDragEvent ) return ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event);

    // keep track of how many units the pointer has been dragged
    BUtilities::Point pos = ev->getPosition();
    dragEdit_ = true ;
    dragUnits_ = std::round(pos.x / unitSize_.first) ;
}


void SequenceNote::unclickedCallback(BEvents::Event* event){
    if( !event ) return ;
    if ( event->getEventType() != BEvents::Event::EventType::buttonReleaseEvent ) return ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event);

    // otherwise, check to see if this event comes with an unclick action
    if (dragEdit_ && ev->getButton() == BDevices::MouseButton::ButtonType::left ){
        dragEdit_ = false ;
        setNumUnits(dragUnits_);
    }
}
