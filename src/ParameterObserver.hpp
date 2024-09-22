#ifndef __PARAMETER_OBSERVER_HPP_
#define __PARAMETER_OBSERVER_HPP_

#include "State.hpp"

#include <lv2/lv2plug.in/ns/ext/urid/urid.h>


/**
 * @brief virtual class to register a class as an ParameterObserver
 *
 * The ParameterObserver class is intended to allow various functions to update based on parameter change events within
 * the ParameterController
 *
 */
class ParameterObserver {
public:
    virtual void onParameterChanged(const StateMapItem* item) = 0 ;
    virtual ~ParameterObserver() = default ;
};

#endif // __PARAMETER_OBSERVER_HPP_
