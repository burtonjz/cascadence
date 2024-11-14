#ifndef __SEQUENCE_NOTE_HPP
#define __SEQUENCE_NOTE_HPP

#include "BWidgets/Button.hpp"
#include "BWidgets/Supports/Draggable.hpp"
#include "BEvents/Event.hpp"
#include "BUtilities/Point.hpp"

#include "uiConfig.hpp"

#include <utility>

class SequenceNote :
    public BWidgets::Button,
    public BWidgets::Draggable
{
private:
    BUtilities::Point<> pos_ ; // widget position (defined by top left corner)
    int numUnits_ ; // number of time units the button will encompass
    int noteIndex_ ; // note index for sequence pattern
    int startTime_ ; // time index for start of note

    bool dragEdit_ ; // track if we are in drag edit mode across BEvents
    int dragUnits_ ; // track the new number of units present across drag actions

public:
    // Constructors

    SequenceNote();

    SequenceNote(
        double x, double y,
        double width = UI_SEQUENCE_NOTE_UNIT_WIDTH,
        double height = UI_SEQUENCE_NOTE_UNIT_HEIGHT,
        int nUnits = 1
    );

    /**
     * @brief get position
     *
     */
    BUtilities::Point<> getPosition() const ;

    /**
     * @brief Set the widget position
     *
     * @param x top left corner x position
     * @param y top left corner y position
     */
    void setPosition(BUtilities::Point<> pt);

    /**
     * @brief Get the number of time units the note encompasses
     *
     */
    int getNumUnits() const ;

    /**
     * @brief Sets the number of time units the sequence note will cover
     *
     * @param units number of units
     */
    void setNumUnits(int units);

    /**
     * @brief Get the note index
     *
     */
    int getNoteIndex() const ;

    /**
     * @brief set the note index
     *
     * @param idx note index
     */
    void setNoteIndex(int idx);

    /**
     * @brief Get the index of the start time unit
     *
     */
    int getStartIndex() const ;

    /**
     * @brief Set the index of the starting time unit
     *
     * @param idx start time unit index
     */
    void setStartIndex(int idx);

    /**
     * @brief return whether the note is in drag mode
     *
     */
    bool isDragMode() const ;

    /**
     * @brief set the drag edit mode
     *
     * @param dragged
     */
    void setDragMode(bool dragged);

private:
    // override BWidgets::Button Button press logic to support more nuanced toggling
    void onButtonPressed (BEvents::Event* event) override ;
    void onButtonReleased (BEvents::Event* event) override ;
    void onButtonClicked (BEvents::Event* event) override ;

};

#endif // __SEQUENCE_NOTE_HPP
