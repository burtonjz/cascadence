#include "ParameterController.hpp"
#include "urids.hpp"

#include <lv2/lv2plug.in/ns/ext/atom/atom.h>
#include <lv2/lv2plug.in/ns/lv2core/lv2_util.h>
#include <lv2/lv2plug.in/ns/ext/atom/util.h>
#include <lv2/lv2plug.in/ns/ext/state/state.h>
#include <lv2/lv2plug.in/ns/ext/parameters/parameters.h>

#include <algorithm>
#include <cstring>

#include "iostream"

ParameterController::ParameterController():
    urids_(nullptr),
    map_(nullptr),
    dict_(),
    State_()
{}

void ParameterController::initialize(const LV2_Feature *const *features, LV2_Atom_Forge* forge, LV2_URID_Map* map, Urids* urids){
    features_ = features ;
    forge_ = forge ;
    map_ = map ;
    urids_ = urids ;

    // initialize state parameter dictionary
    dict_[0].uri  = CASCADENCE__bypass ;
    dict_[0].urid = map_->map(map_->handle, dict_[0].uri) ;
    dict_[0].value = reinterpret_cast<LV2_Atom*>(&State_.bypass) ;
    dict_[0].value->size = sizeof(State_.bypass) - sizeof(LV2_Atom) ;
    dict_[0].value->type = map_->map(map_->handle, LV2_ATOM__Bool) ;

    dict_[1].uri  = CASCADENCE__bpm ;
    dict_[1].urid = map_->map(map_->handle, dict_[1].uri) ;
    dict_[1].value = reinterpret_cast<LV2_Atom*>(&State_.bpm) ;
    dict_[1].value->size = sizeof(State_.bpm) - sizeof(LV2_Atom) ;
    dict_[1].value->type = map_->map(map_->handle, LV2_ATOM__Int) ;

    dict_[2].uri  = CASCADENCE__scaleType ;
    dict_[2].urid = map_->map(map_->handle, dict_[2].uri) ;
    dict_[2].value = reinterpret_cast<LV2_Atom*>(&State_.scaleType) ;
    dict_[2].value->size = sizeof(State_.scaleType) - sizeof(LV2_Atom) ;
    dict_[2].value->type = map_->map(map_->handle, LV2_ATOM__Int) ;

    dict_[3].uri  = CASCADENCE__scaleType ;
    dict_[3].urid = map_->map(map_->handle, dict_[3].uri) ;
    dict_[3].value = reinterpret_cast<LV2_Atom*>(&State_.scaleTonic) ;
    dict_[3].value->size = sizeof(State_.scaleType) - sizeof(LV2_Atom) ;
    dict_[3].value->type = map_->map(map_->handle, LV2_ATOM__Int) ;

}

void ParameterController::registerObserver(ParameterObserver* observer){
    observers_.push_back(observer);
    syncObserver(observer);
}

bool ParameterController::isPatchEvent(const LV2_Atom_Object* obj){
    if (obj->body.otype == urids_->patchGet || obj->body.otype == urids_->patchSet || obj->body.otype == urids_->patchPut) return true ;
    return false ;
}

bool ParameterController::isValidSubject(const LV2_Atom_URID* subject){
    return (!subject ||
        (subject->atom.type == urids_->atomURID && subject->body == urids_->plugin)
    ) ;
}

LV2_State_Status ParameterController::setParameter(LV2_URID key, uint32_t size, LV2_URID type, const void* body) const {
    // find the key in the dictionary
    auto item = std::find_if(
        dict_.begin(),
        dict_.end(),
        [key](const StateMapItem& item){
            return item.urid == key;
        }
    );

    if (item == dict_.end()) return LV2_STATE_ERR_NO_PROPERTY ;

    // make sure the type matches the state item type
    if (item->value->type != type) return LV2_STATE_ERR_BAD_TYPE ;

    // Set state value
    memcpy(item->value + 1, body, size);
    item->value->size = size ;
    notifyObservers(item);

    return LV2_STATE_SUCCESS ;
}

LV2_Atom* ParameterController::getParameter(LV2_URID key) const {
    auto item = std::find_if(
        dict_.begin(),
        dict_.end(),
        [key](const StateMapItem& item){
            return item.urid == key;
        }
    );

    if (item == dict_.end()) return nullptr ;

    return item->value ;
}

void ParameterController::notifyObservers(const StateMapItem* item) const {
    for (ParameterObserver* observer: observers_ ){
        observer->onParameterChanged(item);
    }
}

void ParameterController::syncObserver(ParameterObserver* observer) const {
    for ( size_t i = 0; i < dict_.size(); ++i ){
        observer->onParameterChanged(&dict_[i]);
    }
}

void ParameterController::storeStateProperty(
    LV2_State_Map_Path*      mapPath,
    LV2_State_Status*        saveStatus,
    LV2_State_Store_Function store,
    LV2_State_Handle         handle,
    LV2_URID                 key,
    const LV2_Atom*          value
){
    LV2_State_Status st = LV2_STATE_SUCCESS ;
    if (mapPath && value->type == urids_->atomPath ){
        // create object to store path, of size specified in atom
        const char* path = reinterpret_cast<const char*>(value + 1);
        char* apath = mapPath->abstract_path(mapPath->handle, path);
        size_t pathSize = strlen(apath) + 1 ;

        st = store(
            handle,
            key,
            apath,
            pathSize,
            urids_->atomPath,
            LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE
        );

        free(apath);
    } else {
        // if mapPath not available, store property
        st = store(
            handle,
            key,
            value + 1,
            value->size,
            value->type,
            LV2_STATE_IS_POD | LV2_STATE_IS_PORTABLE
        );
    }

    // update saveStatus if pointer is real
    if (saveStatus && !*saveStatus) *saveStatus = st ;
}

void ParameterController::writeToForge(
    LV2_State_Map_Path* mapPath,
    LV2_URID key,
    const LV2_Atom* value
){
    if (mapPath && value->type == urids_->atomPath ){
        const char* path = reinterpret_cast<const char*>(value + 1);
        char* apath = mapPath->abstract_path(mapPath->handle, path);
        size_t pathSize = strlen(apath) + 1 ;

        // write parameter to forge
        lv2_atom_forge_key(forge_, key);
        lv2_atom_forge_atom(forge_, pathSize , value->type);
        lv2_atom_forge_write(forge_, value, pathSize);

        free(apath);
    } else {
        lv2_atom_forge_key(forge_, key);
        lv2_atom_forge_atom(forge_, value->size, value->type);
        lv2_atom_forge_write(forge_, value, value->size);
    }
}

void ParameterController::restoreStateProperty(
    LV2_State_Status*           status,
    LV2_State_Retrieve_Function retrieve,
    LV2_State_Handle            handle,
    LV2_URID                    key
){
    size_t   vsize  = 0 ;
    uint32_t vtype  = 0 ;
    uint32_t vflags = 0 ;
    const void* value = retrieve(handle, key, &vsize, &vtype, &vflags);

    const LV2_State_Status st = value ? setParameter(key,vsize,vtype,value) : LV2_STATE_ERR_NO_PROPERTY ;

    if (!*status) *status = st ;
}
void ParameterController::handleEvent(const LV2_Atom_Object* obj, long frames){
    if      (obj->body.otype == urids_->patchGet) handleGetEvent(obj, frames ) ;
    else if (obj->body.otype == urids_->patchSet) handleSetEvent(obj, frames) ;
    else if (obj->body.otype == urids_->patchPut) handlePutEvent(obj, frames) ;
}

void ParameterController::handleGetEvent(const LV2_Atom_Object* obj, long frames){

    // get the message
    const LV2_Atom_URID* subject = nullptr ;
    const LV2_Atom_URID* property = nullptr ;

    lv2_atom_object_get(
        obj,
        urids_->patchSubject,  reinterpret_cast<const LV2_Atom**>(&subject),
        urids_->patchProperty, reinterpret_cast<const LV2_Atom**>(&property)
    );

    if(!isValidSubject(subject)) return ;

    if (!property) { // get without specified property, output entire state
        // create property and body atoms
        lv2_atom_forge_frame_time(forge_, frames) ;
        LV2_Atom_Forge_Frame propFrame ;

        lv2_atom_forge_object(forge_, &propFrame, 0, urids_->patchPut);
        lv2_atom_forge_key(forge_, urids_->patchBody);

        LV2_Atom_Forge_Frame bodyFrame;
        lv2_atom_forge_object(forge_, &bodyFrame, 0, 0);

        LV2_State_Map_Path* mapPath = reinterpret_cast<LV2_State_Map_Path*>(lv2_features_data(features_, LV2_STATE__mapPath));
        LV2_State_Status status = LV2_STATE_SUCCESS ;
        for (size_t i = 0; i < dict_.size(); ++i){
            StateMapItem* item = &dict_[i] ;
            writeToForge(mapPath, item->urid, item->value) ;
        }

        lv2_atom_forge_pop(forge_, &bodyFrame) ;
        lv2_atom_forge_pop(forge_,&propFrame) ;
    } else { // get specifies property, output specified property only
        if (property->atom.type != urids_->atomURID) return ;

        const LV2_URID  key   = property->body ;
        const LV2_Atom* value = getParameter(key);

        if (!value) return ;

        // create output atom
        lv2_atom_forge_frame_time(forge_, frames);
        LV2_Atom_Forge_Frame frame ;

        lv2_atom_forge_object(forge_, &frame, 0, urids_->patchSet);
        lv2_atom_forge_key(forge_, urids_->patchProperty);
        lv2_atom_forge_urid(forge_, property->body);

        // store current state
        writeToForge(nullptr,urids_->patchValue,value);
        lv2_atom_forge_pop(forge_, &frame);
    }

}

void ParameterController::handleSetEvent(const LV2_Atom_Object* obj, long frames){
    // get the message
    const LV2_Atom_URID* subject  = nullptr ;
    const LV2_Atom_URID* property = nullptr ;
    const LV2_Atom*      value    = nullptr ;

    lv2_atom_object_get(
        obj,
        urids_->patchSubject,  reinterpret_cast<const LV2_Atom**>(&subject),
        urids_->patchProperty, reinterpret_cast<const LV2_Atom**>(&property),
        urids_->patchValue,    &value,
        0
    );

    if(!isValidSubject(subject)) return ;

    const LV2_URID key = property->body ;
    setParameter(key, value->size, value->type, value + 1);
}

void ParameterController::handlePutEvent(const LV2_Atom_Object* obj, long frames){

}

LV2_State_Status ParameterController::saveState(
        LV2_State_Store_Function  store,
        LV2_State_Handle          handle,
        uint32_t                  flags,
        const LV2_Feature* const* features
){
    LV2_State_Map_Path* mapPath = reinterpret_cast<LV2_State_Map_Path*>(lv2_features_data(features_, LV2_STATE__mapPath));
    LV2_State_Status status = LV2_STATE_SUCCESS ;
    for (size_t i = 0; i < dict_.size(); ++i){
        StateMapItem* item = &dict_[i] ;
        storeStateProperty(mapPath, &status, store, handle, item->urid, item->value) ;
    }

    return status ;
}

LV2_State_Status ParameterController::restoreState(
        LV2_State_Retrieve_Function retrieve,
        LV2_State_Handle            handle,
        uint32_t                    flags,
        const LV2_Feature* const*   features
){
    LV2_State_Status status = LV2_STATE_SUCCESS ;

    for (int i = 0; i < dict_.size(); ++i){
        restoreStateProperty(&status, retrieve, handle, dict_[i].urid);
    }

    return status ;
}
