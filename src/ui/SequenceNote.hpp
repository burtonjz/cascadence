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

public:
    // Constructors

    SequenceNote();

    SequenceNote(double x, double y, int nUnits = 1);

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

private:
    // override BWidgets::Button Button press logic to support more nuanced toggling
    void onButtonPressed (BEvents::Event* event) override ;
    void onButtonReleased (BEvents::Event* event) override ;
    void onButtonClicked (BEvents::Event* event) override ;

};

#endif // __SEQUENCE_NOTE_HPP
