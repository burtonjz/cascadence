#include "SequenceFrame.hpp"
#include "BDevices/MouseButton.hpp"
#include "BEvents/PointerEvent.hpp"
#include "ui/uiConfig.hpp"
// #include "BStyles/Types/Fill.hpp"
#include <string>

SequenceFrame::SequenceFrame(std::string bundlePath, const double x, const double y, const double width, const double height):
    Frame(x,y, width, height),
    bundlePath_(bundlePath),
    noteLabels_(),
    notes_()
{
    // define frame properties
    setClickable(true);
    setDraggable(false);

    // define labels
    for ( size_t i = 0; i < noteLabels_.size(); ++i ){
        add(&noteLabels_[i]);
        double ypos = UI_SEQUENCE_FRAME_HEIGHT - UI_SEQUENCE_FRAME_LABEL_HEIGHT * (i + 1);
        noteLabels_[i].moveTo(0,ypos);
        noteLabels_[i].resize(UI_SEQUENCE_FRAME_LABEL_WIDTH, UI_SEQUENCE_FRAME_LABEL_HEIGHT);
        noteLabels_[i].setText(std::to_string(i));

    }

    // prep SequenceNotes. All are hidden and off screen until click action executed
    for ( size_t i = 0; i < notes_.size(); ++i ){
        add(&notes_[i]);
        notes_[i].setPosition(BUtilities::Point<>(-50,0));
        notes_[i].hide();
    }

    // set bg
    setBackground(BStyles::Fill(bundlePath_ + "/assets/wSequenceFrameBG.png"));

    // define callback functions
    setCallbackFunction(
        BEvents::Event::EventType::buttonClickEvent,
        [this](BEvents::Event* ev){clickedCallback(ev);}
    );
}

void SequenceFrame::clickedCallback(BEvents::Event* event){
    if ( !event ) return ;
    if ( event->getEventType() != BEvents::Event::EventType::buttonClickEvent ) return ;
    std::cout << "Callback triggered " << std::endl ;

    BEvents::PointerEvent* ev = dynamic_cast<BEvents::PointerEvent*>(event) ;

    if ( ev->getButton() != BDevices::MouseButton::ButtonType::left ) return ;

    auto pt = ev->getPosition() ;
    if ( clickedEmptySlot(pt) ){
        size_t i = 0 ;
        for ( ; i < notes_.size() ; ++i ){
            if ( ! notes_[i].isVisible() ) break ;
        }
        if ( i == notes_.size() - 1 ){
            std::cout << "[CascadenceUI::SequenceFrame] Cannot add new note, hit max capacity." << std::endl ;
            return ;
        }

        // populate the note
        std::cout << "point_clicked: " << pt.x << ", " << pt.y ;
        auto dest = getNearestSlotPoint(pt);
        notes_[i].setStartIndex(dest.x);
        notes_[i].setNoteIndex(dest.y);
        pt.x = static_cast<int>(dest.x) * UI_SEQUENCE_NOTE_UNIT_WIDTH + UI_SEQUENCE_FRAME_LABEL_WIDTH ;
        pt.y = UI_SEQUENCE_FRAME_HEIGHT - UI_SEQUENCE_NOTE_UNIT_HEIGHT * (static_cast<int>(dest.y) + 1);
        std::cout << ", start_index: " << dest.x << ", note_index: " << dest.y << ", dest_point: " << pt.x << ", " << pt.y << std::endl ;
        notes_[i].setPosition(pt);
        notes_[i].show();
    }
}

bool SequenceFrame::clickedEmptySlot(BUtilities::Point<> pt) const {
    if (pt.x <= UI_SEQUENCE_FRAME_LABEL_WIDTH ) return false ;

    for ( size_t i = 0 ; i < notes_.size() ; ++i ){
        if ( notes_[i].isVisible() ){
            BUtilities::Point<> np = notes_[i].getPosition() ;
            if (
                np.x < pt.x && pt.x < np.x + notes_[i].getNumUnits() * UI_SEQUENCE_NOTE_UNIT_WIDTH &&
                np.y < pt.y && pt.y < np.y + UI_SEQUENCE_NOTE_UNIT_HEIGHT
            ){
                return false ;
            }
        }
    }
    return true ;
}

BUtilities::Point<> SequenceFrame::getNearestSlotPoint(BUtilities::Point<> pt) const {
    pt.x = ( pt.x - UI_SEQUENCE_FRAME_LABEL_WIDTH ) / UI_SEQUENCE_NOTE_UNIT_WIDTH ;
    pt.y = noteLabels_.size() - 1 - static_cast<int>( pt.y / UI_SEQUENCE_NOTE_UNIT_HEIGHT) ;

    return pt ;
}
