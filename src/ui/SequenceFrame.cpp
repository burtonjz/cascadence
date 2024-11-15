#include "SequenceFrame.hpp"
#include "BDevices/MouseButton.hpp"
#include "BEvents/PointerEvent.hpp"
#include "ui/uiConfig.hpp"
// #include "BStyles/Types/Fill.hpp"
#include <cstdint>
#include <string>
#include <sys/types.h>

SequenceFrame::SequenceFrame(std::string bundlePath, const double x, const double y, const double width, const double height):
    Frame(x,y, width, height),
    bundlePath_(bundlePath),
    notes_(),
    dragMode_(DragMode::DRAG_OFF)
{
    // define frame properties
    setClickable(true);
    setDraggable(false);

    // // define labels
    // for ( size_t i = 0; i < noteLabels_.size(); ++i ){
    //     add(&noteLabels_[i]);
    //     double ypos = UI_SEQUENCE_FRAME_HEIGHT - UI_SEQUENCE_FRAME_LABEL_HEIGHT * (i + 1);
    //     noteLabels_[i].moveTo(0,ypos);
    //     noteLabels_[i].resize(UI_SEQUENCE_FRAME_LABEL_WIDTH, UI_SEQUENCE_FRAME_LABEL_HEIGHT);
    //     noteLabels_[i].setText(std::to_string(i));

    // }

    // prep SequenceNotes. All are hidden and off screen until click action executed
    for ( size_t i = 0; i < notes_.size(); ++i ){
        add(&notes_[i]);
        notes_[i].setPosition(BUtilities::Point<>(-50,0));
        notes_[i].hide();
        notes_[i].setCallbackFunction(
            BEvents::Event::EventType::pointerDragEvent,
            [this](BEvents::Event* ev){noteDraggedCallback(ev);}
        );

        notes_[i].setCallbackFunction(
            BEvents::Event::EventType::buttonEvents,
            [this](BEvents::Event* ev){noteButtonCallback(ev);}
        );
    }

    // set bg
    setBackground(BStyles::Fill(bundlePath_ + "/assets/wSequenceFrameBG.png"));

    // set frame callback functions
    setCallbackFunction(
        BEvents::Event::EventType::buttonPressEvent,
        [this](BEvents::Event* ev){frameClickedCallback(ev);}
    );
}

void SequenceFrame::frameClickedCallback(BEvents::Event* event){
    if ( !event ) return ;
    if ( static_cast<uint32_t>(event->getEventType()) & static_cast<uint32_t>(BEvents::Event::EventType::pointerEvents) ) return ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event) ;

    if ( ev->getButton() == BDevices::MouseButton::ButtonType::left ) populateNote(ev) ;
}

void SequenceFrame::noteDraggedCallback(BEvents::Event* event){
    if ( !event ) return ;
    if ( event->getEventType() != BEvents::Event::EventType::pointerDragEvent ) return ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event);
    SequenceNote* note = dynamic_cast<SequenceNote*>(ev->getWidget());

    BUtilities::Point pos = ev->getPosition();

    switch ( dragMode_ ){
    case DragMode::DRAG_OFF:
        setNoteDragMode(note, pos);
        break ;
    case DragMode::DRAG_LEFT:
        updateNoteDragLeft(note,pos);
        break ;
    case DragMode::DRAG_RIGHT:
        updateNoteDragRight(note,pos);
        break ;
    case DragMode::DRAG_NA:
        break ;
    }
}


void SequenceFrame::noteButtonCallback(BEvents::Event* event){
    if( !event ) return ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event);
    SequenceNote* note = dynamic_cast<SequenceNote*>(ev->getWidget());
    switch(event->getEventType()){
    case BEvents::Event::EventType::buttonClickEvent:
        if (!note->getValue()) setActiveNote(note);
        else note->setValue(false);
        break ;
    case BEvents::Event::EventType::buttonPressEvent:
        break ;
    case BEvents::Event::EventType::buttonReleaseEvent:
        dragMode_ = DragMode::DRAG_OFF ;
        break ;
    default:
        break ;
    }
    if ( event->getEventType() == BEvents::Event::EventType::buttonReleaseEvent ) {

    }
}

void SequenceFrame::populateNote(BEvents::PointerEvent* ev){
    auto pt = ev->getPosition() ;
    SequenceNote* note = getNoteAtPoint(pt);
    if (!note){
        size_t i = 0 ;
        for ( ; i < notes_.size() ; ++i ){
            if ( ! notes_[i].isVisible() ) break ;
        }
        if ( i == notes_.size() - 1 ){
            std::cout << "[CascadenceUI::SequenceFrame] Cannot add new note, hit max capacity." << std::endl ;
            return ;
        }

        // populate the note
        auto dest = getNearestSlotPoint(pt);
        notes_[i].setStartIndex(dest.x);
        notes_[i].setNoteIndex(dest.y);
        pt.x = static_cast<int>(dest.x) * UI_SEQUENCE_NOTE_UNIT_WIDTH ;
        pt.y = UI_SEQUENCE_FRAME_HEIGHT - UI_SEQUENCE_NOTE_UNIT_HEIGHT * (static_cast<int>(dest.y) + 1);
        notes_[i].setPosition(pt);
        notes_[i].show();
        setActiveNote(i);

    }
}

void SequenceFrame::setActiveNote(size_t i){
    for (size_t j = 0 ; j < notes_.size() ; ++j){
        if ( notes_[j].isVisible() && j != i ) notes_[j].setValue(false);
    }
    notes_[i].setValue(true);
}
void SequenceFrame::setActiveNote(SequenceNote* note){
    for (size_t j = 0 ; j < notes_.size() ; ++j){
        if ( notes_[j].isVisible() && &notes_[j] != note ) notes_[j].setValue(false);
    }
    note->setValue(true);
}

void SequenceFrame::setNoteDragMode(SequenceNote* note, BUtilities::Point<> pt){
    double buffer = .2 ; // 20% of units closeness to edge

    if ( pt.x <  UI_SEQUENCE_NOTE_UNIT_WIDTH * buffer ){
        dragMode_ = DragMode::DRAG_LEFT ;
        return ;
    }

    double rightSideX = UI_SEQUENCE_NOTE_UNIT_WIDTH * note->getNumUnits() ;
    if ( pt.x > rightSideX - UI_SEQUENCE_NOTE_UNIT_WIDTH * buffer ){
        dragMode_ = DragMode::DRAG_RIGHT ;
        return ;
    }

    // just a random drag
    dragMode_ = DragMode::DRAG_NA ;
}

void SequenceFrame::updateNoteDragLeft(SequenceNote* note, BUtilities::Point<> pt){
    double relRightX = UI_SEQUENCE_NOTE_UNIT_WIDTH * note->getNumUnits() ;
    if ( pt.x > relRightX ) return ;

    int dUnits = std::round(pt.x / UI_SEQUENCE_NOTE_UNIT_WIDTH);
    if ( dUnits == 0 ) return ;

    auto newPos = note->getPosition() ; // position relative to frame
    newPos.x += UI_SEQUENCE_NOTE_UNIT_WIDTH * dUnits;

    // handle left boundary
    if (newPos.x < 0 ){
        newPos.x = 0 ;
    }

    int absLeftUnits = newPos.x / UI_SEQUENCE_NOTE_UNIT_WIDTH ;
    int absRightUnits = (note->getPosition().x + relRightX) / UI_SEQUENCE_NOTE_UNIT_WIDTH ;
    int newUnits = absRightUnits - absLeftUnits ;

    std::cout << "pos.x=" << note->getPosition().x
        << ", newPos.x=" << newPos.x
        << ", pt.x=" << pt.x
        << ", nUnits=" << note->getNumUnits()
        << ", dUnits=" << dUnits
        << ", absLeftUnits=" << absLeftUnits
        << ", absRightUnits=" << absRightUnits
        << ", newUnits=" << newUnits
        << std::endl ;

    if (newUnits > 0){
        note->setPosition(newPos);
        note->setNumUnits(newUnits);
        emitExposeEvent();
    }
}

void SequenceFrame::updateNoteDragRight(SequenceNote* note, BUtilities::Point<> pt){
    if ( pt.x < 0 ) return ;
    int originIndexInUnits = note->getPosition().x / UI_SEQUENCE_NOTE_UNIT_WIDTH ;
    int newUnits = std::round(pt.x / UI_SEQUENCE_NOTE_UNIT_WIDTH) ;
    // handle right boundary
    if (originIndexInUnits + newUnits > UI_SEQUENCE_MAX_TIME_UNITS){
        newUnits = UI_SEQUENCE_MAX_TIME_UNITS - originIndexInUnits ;
    }
    if (newUnits > 0) note->setNumUnits(newUnits);
    emitExposeEvent();
}

SequenceNote* SequenceFrame::getNoteAtPoint(BUtilities::Point<> pt){
    for ( size_t i = 0 ; i < notes_.size() ; ++i ){
        if ( notes_[i].isVisible() ){
            BUtilities::Point<> np = notes_[i].getPosition() ;
            if (
                np.x < pt.x && pt.x < np.x + notes_[i].getNumUnits() * UI_SEQUENCE_NOTE_UNIT_WIDTH &&
                np.y < pt.y && pt.y < np.y + UI_SEQUENCE_NOTE_UNIT_HEIGHT
            ){
                return &notes_[i] ;
            }
        }
    }
    return nullptr ;
}

BUtilities::Point<> SequenceFrame::getNearestSlotPoint(BUtilities::Point<> pt) const {
    pt.x = pt.x / UI_SEQUENCE_NOTE_UNIT_WIDTH ;
    pt.y = UI_SEQUENCE_FRAME_NUM_NOTES - 1 - static_cast<int>( pt.y / UI_SEQUENCE_NOTE_UNIT_HEIGHT) ;
    return pt ;
}
