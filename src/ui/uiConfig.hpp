#ifndef __UI_CONFIG_HPP_
#define __UI_CONFIG_HPP_

#include "BStyles/Types/Color.hpp"

constexpr BStyles::Color UI_SEQUENCE_NOTE_MIDI_COLOR = BStyles::Color(0.0,0.0,1.0) ;

constexpr int UI_WIDTH = 1200 ;
constexpr int UI_HEIGHT = 800 ;

// Basic Widgets Positioning
constexpr int UI_BYPASS_X      = 10 ;
constexpr int UI_BYPASS_Y      = 10 ;
constexpr int UI_BYPASS_WIDTH  = 200 ;
constexpr int UI_BYPASS_HEIGHT = 100 ;

constexpr int UI_BPM_X      = 220 ;
constexpr int UI_BPM_Y      = 10 ;
constexpr int UI_BPM_WIDTH  = 50 ;
constexpr int UI_BPM_HEIGHT = 100 ;

constexpr int UI_SCALE_TYPE_X      = 10 ;
constexpr int UI_SCALE_TYPE_Y      = 110 ;
constexpr int UI_SCALE_TYPE_WIDTH  = 100 ;
constexpr int UI_SCALE_TYPE_HEIGHT = 50 ;

constexpr int UI_SCALE_TONIC_X      = 120 ;
constexpr int UI_SCALE_TONIC_Y      = 110 ;
constexpr int UI_SCALE_TONIC_WIDTH  = 100 ;
constexpr int UI_SCALE_TONIC_HEIGHT = 50 ;

// SEQUENCE FRAME POSITIONING
constexpr int UI_SEQUENCE_FRAME_X = 275 ;
constexpr int UI_SEQUENCE_FRAME_Y = 50 ;

constexpr int UI_SEQUENCE_FRAME_WIDTH  = 850 ;
constexpr int UI_SEQUENCE_FRAME_HEIGHT = 650 ;

constexpr int UI_SEQUENCE_FRAME_LABEL_WIDTH  = 50 ;
constexpr int UI_SEQUENCE_FRAME_LABEL_HEIGHT = 50 ;

constexpr int UI_SEQUENCE_NOTE_UNIT_WIDTH  = 50 ;
constexpr int UI_SEQUENCE_NOTE_UNIT_HEIGHT = 50 ;

constexpr int UI_SEQUENCE_FRAME_NUM_NOTES = 13 ;
constexpr int UI_SEQUENCE_MAX_TIME_UNITS = 16 ;

#endif // __UI_CONFIG_HPP_
