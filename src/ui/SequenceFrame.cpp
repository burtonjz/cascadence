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
    notes_()
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

    // keep track of how many units the pointer has been dragged
    if ( !note->isDragMode() ){
        note->setDragMode(true) ;
    }

    BUtilities::Point pos = ev->getPosition();
    note->setNumUnits(std::round(pos.x / UI_SEQUENCE_NOTE_UNIT_WIDTH));
    emitExposeEvent();
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
