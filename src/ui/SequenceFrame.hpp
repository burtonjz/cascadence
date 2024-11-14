#ifndef __UI_SEQUENCE_FRAME_
#define __UI_SEQUENCE_FRAME_

#include "BWidgets/Frame.hpp"
#include "BWidgets/Label.hpp"
#include "BUtilities/Point.hpp"
#include "BWidgets/Supports/Clickable.hpp"

#include "uiConfig.hpp"
#include "config.hpp"
#include "SequenceNote.hpp"

#include <array>
#include <string>

/*
TODO:

- grid background
- labels for note position (should dynamically update with scale/key eventually)
- logic to populate a sequence note on clicked grid position
- logic to remove/hide a sequence note when specified key or button press occurs
- logic to "max out" sequence notes based on sequence max duration
- callback to send data to backend
- maybe a save/edit button so we aren't sending in real time?
*/

class SequenceFrame :
    public BWidgets::Frame,
    public BWidgets::Clickable
{

private:
    std::string bundlePath_ ;
    // BWidgets::Image wBgImage_ ;
    // std::array<BWidgets::Label, 13> noteLabels_ ; // contain full chromatic scale including octave
    std::array<SequenceNote,CONFIG_MAX_SEQUENCE_SIZE> notes_ ;
    BEvents::Event::EventType lastFrameEvent_ ;

public:
    /**
     * @brief Construct a new Sequence Frame object
     *
     * @param bundlePath path to assets folder
     * @param x top left x coordinate
     * @param y top left y coordinate
     * @param width frame width
     * @param height frame height
     */
    SequenceFrame(
        std::string bundlePath,
        const double x      = UI_SEQUENCE_FRAME_X,
        const double y      = UI_SEQUENCE_FRAME_Y,
        const double width  = UI_SEQUENCE_FRAME_WIDTH,
        const double height = UI_SEQUENCE_FRAME_HEIGHT
    );

    /**
     * @brief callback function for clicking action
     *
     * @param event Passed event
     */
    void frameClickedCallback(BEvents::Event* event);

    /**
     * @brief callback function for dragging action
     *
     * @param event Passed Event.
     */
    void noteDraggedCallback(BEvents::Event* event);

    /**
     * @brief callback function for note button pressing actions
     *
     * @param event Passed Event.
     */
    void noteButtonCallback(BEvents::Event* event);

private:
    void populateNote(BEvents::PointerEvent* ev) ;

    void setActiveNote(size_t i);
    void setActiveNote(SequenceNote* note);

    SequenceNote* getNoteAtPoint(BUtilities::Point<> pt);

    BUtilities::Point<> getNearestSlotPoint(BUtilities::Point<> pt) const ;
};

#endif // __UI_SEQUENCE_FRAME_
