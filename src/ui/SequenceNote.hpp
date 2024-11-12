#ifndef __SEQUENCE_NOTE_HPP
#define __SEQUENCE_NOTE_HPP

#include "BWidgets/Widget.hpp"
#include "BWidgets/Button.hpp"
#include "BWidgets/Supports/Draggable.hpp"
#include "BEvents/Event.hpp"

#include "uiConfig.hpp"

#include <utility>

/*
Plan:

Create a button that represents a sequence note.
Position is defined by top left corner.
Define a beat size with the x,y size
Size of the widget will be defined

*/


class SequenceNote :
    public BWidgets::Button,
    public BWidgets::Draggable
{
private:
    std::pair<int, int> pos_ ; // widget position (defined by top left corner)
    std::pair<int, int> unitSize_ ; // size of a given unit
    int numUnits_ ; // number of time units the button will encompass

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
     * @brief set the size of the object for the smallest unit (16th note)
     *
     * @param width width
     * @param height height
     */
    void setUnitSize(const double width, const double height);

    /**
     * @brief Set the widget position
     *
     * @param x top left corner x position
     * @param y top left corner y position
     */
    void setPosition(const double x, const double y);

    /**
     * @brief Sets the number of time units the sequence note will cover
     *
     * @param units number of units
     */
    void setNumUnits(int units);

    /**
     * @brief callback function for dragging action
     *
     * @param event Passed Event.
     */
    void draggedCallback(BEvents::Event* event);

    /**
     * @brief callback function for clicking action
     *
     * @param event Passed Event.
     */
    void unclickedCallback(BEvents::Event* event);
};



#endif // __SEQUENCE_NOTE_HPP
